#include "Inspector.h"
#include <iostream>

using namespace mnsx::skuld;

// 静态成员初始化
bool Inspector::isAiLoaded_ = false;
std::unique_ptr<Ort::Env> Inspector::env_ = nullptr;
std::unique_ptr<Ort::Session> Inspector::session_ = nullptr;
Ort::MemoryInfo Inspector::memoryInfo_ = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);

bool Inspector::initAIEngine(const std::string& onnxPath) {
    try {
        env_.reset(new Ort::Env(ORT_LOGGING_LEVEL_WARNING, "SkuldVision"));
        Ort::SessionOptions sessionOptions;
        sessionOptions.SetIntraOpNumThreads(2); // 增加到 2 线程，提升工控机性能
        sessionOptions.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_ALL);

#ifdef _WIN32
        std::wstring wPath(onnxPath.begin(), onnxPath.end());
        session_.reset(new Ort::Session(*env_, wPath.c_str(), sessionOptions));
#else
        session_.reset(new Ort::Session(*env_, onnxPath.c_str(), sessionOptions));
#endif
        isAiLoaded_ = true;
        std::cout << "✅ Skuld 神经网络已就绪 (ORT 引擎)" << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "❌ 引擎装载失败: " << e.what() << std::endl;
        return false;
    }
}

void Inspector::extractAnomaliesYOLO(const cv::Mat& src,
                                     std::vector<cv::Rect>& outRects,
                                     std::vector<int>& outClassIds,
                                     float confThreshold) {
    outRects.clear();
    outClassIds.clear();

    if (!isAiLoaded_ || src.empty()) return;

    // =========================
    // 1. LetterBox 预处理
    // =========================
    struct LetterBoxInfo {
        float scale;
        int pad_w;
        int pad_h;
    } lb;

    int target = 640;
    int w = src.cols;
    int h = src.rows;

    float scale = std::min(target / (float)w, target / (float)h);
    int new_w = int(w * scale);
    int new_h = int(h * scale);

    cv::Mat resized;
    cv::resize(src, resized, cv::Size(new_w, new_h));

    int pad_w = target - new_w;
    int pad_h = target - new_h;

    int top = pad_h / 2;
    int left = pad_w / 2;

    cv::Mat inputImg;
    cv::copyMakeBorder(resized, inputImg,
                       top, pad_h - top,
                       left, pad_w - left,
                       cv::BORDER_CONSTANT,
                       cv::Scalar(114, 114, 114));

    lb.scale = scale;
    lb.pad_w = left;
    lb.pad_h = top;

    // =========================
    // 2. 转 blob
    // =========================
    cv::Mat blob;
    cv::dnn::blobFromImage(inputImg, blob, 1.0 / 255.0,
                           cv::Size(640, 640),
                           cv::Scalar(), true, false);

    size_t inputTensorSize = 1 * 3 * 640 * 640;
    std::vector<int64_t> inputDims = {1, 3, 640, 640};

    Ort::Value inputTensor = Ort::Value::CreateTensor<float>(
        memoryInfo_, (float*)blob.data, inputTensorSize,
        inputDims.data(), inputDims.size());

    const char* inputNames[] = {"images"};
    const char* outputNames[] = {"output0"};

    // =========================
    // 3. 推理
    // =========================
    auto outputTensors = session_->Run(
        Ort::RunOptions{nullptr},
        inputNames, &inputTensor, 1,
        outputNames, 1);

    float* rawData = outputTensors[0].GetTensorMutableData<float>();

    // shape: [1, 84, 8400]
    int num_channels = 84;
    int num_preds = 8400;

    float x_factor = 1.0f / lb.scale;
    float y_factor = 1.0f / lb.scale;

    std::vector<cv::Rect> boxes;
    std::vector<float> confidences;
    std::vector<int> classIds;

    // =========================
    // 4. 正确解析（CHW）
    // =========================
    for (int i = 0; i < num_preds; ++i) {

        float cx = rawData[0 * num_preds + i];
        float cy = rawData[1 * num_preds + i];
        float w  = rawData[2 * num_preds + i];
        float h  = rawData[3 * num_preds + i];

        float maxScore = 0.0f;
        int classId = -1;

        for (int c = 4; c < num_channels; ++c) {

            float raw = rawData[c * num_preds + i];

            // 🔥 加 sigmoid
            float score = rawData[c * num_preds + i];

            if (score > maxScore) {
                maxScore = score;
                classId = c - 4;
            }
        }

        std::cout << "msxScore: " << maxScore << std::endl;
        if (maxScore > confThreshold) {

            float x = cx - 0.5f * w;
            float y = cy - 0.5f * h;

            // 去 padding
            x -= lb.pad_w;
            y -= lb.pad_h;

            // 还原
            x *= x_factor;
            y *= y_factor;
            w *= x_factor;
            h *= y_factor;

            boxes.emplace_back((int)x, (int)y, (int)w, (int)h);
            confidences.emplace_back(maxScore);
            classIds.emplace_back(classId);
        }
    }

    // =========================
    // 5. NMS
    // =========================
    std::vector<int> indices;
    cv::dnn::NMSBoxes(boxes, confidences, confThreshold, 0.45f, indices);

    for (int idx : indices) {
        outRects.push_back(boxes[idx]);
        outClassIds.push_back(classIds[idx]);
    }
}