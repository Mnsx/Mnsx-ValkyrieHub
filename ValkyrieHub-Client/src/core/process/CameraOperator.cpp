/** 
 * @file CameraOperator.cpp
 * @author Mnsx_x <xx1527030652@gmail.com>
 * @date 2026/4/22
 * @description 
 */
#include <exception>

#include "core/process/CameraOperator.h"
#include "KrakenPool.h"
#include "Logger.h"
#include "SkuldVision.h"
#include "utils/NetworkDetector.h"

using namespace mnsx::valkyrie;
using namespace mnsx::hermes;
using namespace mnsx::skuld;

std::string serializeRects(const std::vector<cv::Rect>& rects) {
    if (rects.empty()) return "";

    std::stringstream ss;
    for (size_t i = 0; i < rects.size(); ++i) {
        ss << rects[i].x << ","
           << rects[i].y << ","
           << rects[i].width << ","
           << rects[i].height;
        if (i != rects.size() - 1) {
            ss << ";";
        }
    }
    return ss.str();
}

std::string serializeTypes(const std::vector<AnomalyType>& types) {
    if (types.empty()) {
        return "";
    }
    std::stringstream ss;
    for (size_t i = 0; i < types.size(); ++i) {
        ss << static_cast<int>(types[i]);
        if (i != types.size() - 1) {
            ss << ";";
        }
    }
    return ss.str();
}

DataStream CameraOperator::process() {
    // 拍摄产品照片
    // 通过Opencv打开摄像头
    // cv::VideoCapture cap;
    // int deviceID = 0;
    // int apiID = cv::CAP_V4L2;
    // cap.open(deviceID, apiID);
    // if (!cap.isOpened()) {
    //     cap.open(1, apiID); // 尝试备用通道
    // }
    // if (!cap.isOpened()) {
    // }
    // // 防止超时
    // cap.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));
    // cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    // cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
    // cap.set(cv::CAP_PROP_FPS, 30);
    // // 将摄像头的一帧画像保存到Mat中
    // cv::Mat frame;
    // for (int i = 0; i < 10; ++i) {
    //     cap >> frame;
    //     if (!frame.empty()) {
    //         break; // 成功抓到画面，立刻跳出循环
    //     }
    //     std::this_thread::sleep_for(std::chrono::milliseconds(100));
    // }

    // TODO 模拟数据
    std::string mockImagePath = "./static/mock_defect_03.jpg";
    cv::Mat frame = cv::imread(mockImagePath, cv::IMREAD_COLOR);

    // 将图片任务投入线程池中进行分析
    if (!frame.empty()) {
        pool_->enqueue([this, frame]() {
            DataStream res = imageAnalysis(frame);

        });
    }
    DataStream res_data_stream = DataStream("");
    return res_data_stream;
}

DataStream CameraOperator::imageAnalysis(cv::Mat async_mat) {
    cv::Mat mat = async_mat.clone();
    // 调用Skuld对图像进行检测
    InspectResult res = skuld_vision_.process(mat);

    // 如果通过检测
    if (res.passed == true) {
        DataStream res_data_stream = DataStream("");
        return res_data_stream;
    }

    // 线程池中将检测出异常的图片，转换格式，装填到DataStream中，返回
    if (res.status == SkuldStatus::SUCCESS) {
        return sendBadImageData(mat, res);
    } else {
        // 出现异常，打印日志
        LOG_ERROR << "远端算法出现异常";
        throw std::runtime_error("...");
    }
}

DataStream CameraOperator::sendBadImageData(cv::Mat mat, InspectResult &res) {
    // 将图片转换为jpg格式
    std::vector<uchar> buf;
    std::vector<int> params = {cv::IMWRITE_JPEG_QUALITY, 90};
    // 将 Mat 转换为 JPG 字节流，存入 vector
    cv::imencode(".jpg", mat, buf, params);
    // 准备数据
    std::shared_ptr<DataStream> res_data_stream = std::make_shared<DataStream>("");
    // 准备标识符Mac地址
    NodeNetworkInfo info = NetworkDetector::getPrimaryNetworkInfo();
    std::string anomalyInfo = std::to_string(res.anomalyCount) + "|" + serializeRects(res.anomalyRects) + "|" + serializeTypes(res.anomalyTypes);
    // 将 byte 数组转为 string 容器，完美塞入 DataStream
    std::string image_bytes(buf.begin(), buf.end());
    std::string res_info = info.mac + "|" + anomalyInfo;
    *(res_data_stream.get()) << res_info << image_bytes;
    rpc_client_->call("DefectApproval.postAnomalyInfo", res_data_stream.get()->data(), nullptr);

    return *(res_data_stream.get());
}
