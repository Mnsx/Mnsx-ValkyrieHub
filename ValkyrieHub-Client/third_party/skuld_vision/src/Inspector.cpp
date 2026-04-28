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

// void Inspector::extractAnomaliesYOLO(const cv::Mat& src,
//                                      std::vector<cv::Rect>& outRects,
//                                      std::vector<int>& outClassIds,
//                                      float confThreshold) {
//
//     outRects.clear();
//     outClassIds.clear();
//
//     if (!isAiLoaded_ || src.empty()) return;
//
//     // =========================
//     // 0. 修复单通道致命坑 (NEU-DET 专属修复)
//     // =========================
//     // 解决 NEU-DET 单通道灰度图导致的内存错位问题
//     cv::Mat processImg = src.clone();
//     if (processImg.channels() == 1) {
//         cv::cvtColor(processImg, processImg, cv::COLOR_GRAY2BGR);
//     } else if (processImg.channels() == 4) {
//         cv::cvtColor(processImg, processImg, cv::COLOR_BGRA2BGR);
//     }
//
//     // =========================
//     // 1. LetterBox 预处理
//     // =========================
//     struct LetterBoxInfo {
//         float scale;
//         int pad_w;
//         int pad_h;
//     } lb;
//
//     int target = 640;
//     int w = processImg.cols;
//     int h = processImg.rows;
//
//     float scale = std::min(target / (float)w, target / (float)h);
//     int new_w = int(w * scale);
//     int new_h = int(h * scale);
//
//     cv::Mat resized;
//     cv::resize(processImg, resized, cv::Size(new_w, new_h));
//
//     int pad_w = target - new_w;
//     int pad_h = target - new_h;
//
//     int top = pad_h / 2;
//     int left = pad_w / 2;
//
//     cv::Mat inputImg;
//     cv::copyMakeBorder(resized, inputImg,
//                        top, pad_h - top,
//                        left, pad_w - left,
//                        cv::BORDER_CONSTANT,
//                        cv::Scalar(114, 114, 114));
//
//     lb.scale = scale;
//     lb.pad_w = left;
//     lb.pad_h = top;
//
//     // =========================
//     // 2. 转 blob
//     // =========================
//     cv::Mat blob;
//     cv::dnn::blobFromImage(inputImg, blob, 1.0 / 255.0,
//                            cv::Size(640, 640),
//                            cv::Scalar(), true, false);
//
//     size_t inputTensorSize = 1 * 3 * 640 * 640;
//     std::vector<int64_t> inputDims = {1, 3, 640, 640};
//
//     Ort::Value inputTensor = Ort::Value::CreateTensor<float>(
//         memoryInfo_, (float*)blob.data, inputTensorSize,
//         inputDims.data(), inputDims.size());
//
//     const char* inputNames[] = {"images"};
//     const char* outputNames[] = {"output0"};
//
//     // =========================
//     // 3. 推理
//     // =========================
//     auto outputTensors = session_->Run(
//         Ort::RunOptions{nullptr},
//         inputNames, &inputTensor, 1,
//         outputNames, 1);
//
//     float* rawData = outputTensors[0].GetTensorMutableData<float>();
//     Ort::TensorTypeAndShapeInfo shapeInfo = outputTensors[0].GetTensorTypeAndShapeInfo();
//     std::vector<int64_t> outputShape = shapeInfo.GetShape();
//
//     // 动态适配 [1, 10, 8400] 和 [1, 8400, 10] 两种导出格式
//     int num_channels = outputShape[1];
//     int num_preds = outputShape[2];
//     bool isTransposed = false;
//
//     if (num_channels > num_preds) { // 如果是 [1, 8400, 10]
//         std::swap(num_channels, num_preds);
//         isTransposed = true;
//     }
//
//     float x_factor = 1.0f / lb.scale;
//     float y_factor = 1.0f / lb.scale;
//
//     std::vector<cv::Rect> boxes;
//     std::vector<float> confidences;
//     std::vector<int> classIds;
//
//     // =========================
//     // 4. 正确解析
//     // =========================
//     for (int i = 0; i < num_preds; ++i) {
//         float cx, cy, w, h;
//
//         // 兼容两种不同的内存步长
//         if (!isTransposed) {
//             cx = rawData[0 * num_preds + i];
//             cy = rawData[1 * num_preds + i];
//             w  = rawData[2 * num_preds + i];
//             h  = rawData[3 * num_preds + i];
//         } else {
//             cx = rawData[i * num_channels + 0];
//             cy = rawData[i * num_channels + 1];
//             w  = rawData[i * num_channels + 2];
//             h  = rawData[i * num_channels + 3];
//         }
//
//         float maxScore = 0.0f;
//         int classId = -1;
//
//         for (int c = 4; c < num_channels; ++c) {
//             float raw = (!isTransposed) ? rawData[c * num_preds + i] : rawData[i * num_channels + c];
//
//             // 绝大多数标准 YOLOv8 导出已经包含了 Sigmoid，可以直接使用 raw
//             // 如果你 debug 发现 raw 出现了负数或大于 1 的数，请把下面这行换成:
//             // float score = 1.0f / (1.0f + std::exp(-raw));
//             float score = raw;
//
//             if (score > maxScore) {
//                 maxScore = score;
//                 classId = c - 4;
//             }
//         }
//
//         if (maxScore > confThreshold) {
//             float x = cx - 0.5f * w;
//             float y = cy - 0.5f * h;
//
//             // 去 padding
//             x -= lb.pad_w;
//             y -= lb.pad_h;
//
//             // 还原
//             x *= x_factor;
//             y *= y_factor;
//             w *= x_factor;
//             h *= y_factor;
//
//             boxes.emplace_back((int)x, (int)y, (int)w, (int)h);
//             confidences.emplace_back(maxScore);
//             classIds.emplace_back(classId);
//         }
//     }
//
//     // =========================
//     // 5. NMS
//     // =========================
//     std::vector<int> indices;
//     // 适当调低 NMS 阈值，YOLOv8 通常推荐 0.45 甚至更低
//     cv::dnn::NMSBoxes(boxes, confidences, confThreshold, 0.45f, indices);
//
//     for (int idx : indices) {
//         outRects.push_back(boxes[idx]);
//         outClassIds.push_back(classIds[idx]);
//     }
// }

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

    // 动态获取当前模型的输出维度，适配任何数据集
    Ort::TensorTypeAndShapeInfo shapeInfo = outputTensors[0].GetTensorTypeAndShapeInfo();
    std::vector<int64_t> outputShape = shapeInfo.GetShape();

    int num_channels = outputShape[1]; // NEU-DET 会自动识别为 10
    int num_preds = outputShape[2];    // 自动识别预测框数量，通常为 8400

    for (auto s : outputShape)
        std::cout << s << " ";

    for (int i = 0; i < 20; ++i)
        std::cout << rawData[i] << std::endl;

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

        // if (w < 5 || h < 5) continue;

        float maxScore = 0.0f;
        int classId = -1;

        for (int c = 4; c < num_channels; ++c) {

            float score = rawData[c * num_preds + i];  // ✅ 直接用
            // float raw = rawData[c * num_preds + i];
            //
            // // 🔥 加 sigmoid
            // float score = rawData[c * num_preds + i];

            if (score > maxScore) {
                maxScore = score;
                classId = c - 4;
            }
            std::cout << ".........." << std::endl;
            float raw = rawData[c * num_preds + i];
            std::cout << raw << " ";
        }
        std::cout << std::endl;


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
    cv::dnn::NMSBoxes(boxes, confidences, confThreshold, 0.25f, indices);

    for (int idx : indices) {
        outRects.push_back(boxes[idx]);
        outClassIds.push_back(classIds[idx]);
    }
}