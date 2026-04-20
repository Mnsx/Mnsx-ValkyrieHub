/**
 * @file SkuldVision.cpp
 * @author Mnsx_x <xx1527030652@gmail.com>
 * @date 2026/4/15
 */
#include "SkuldVision.h"
#include "Inspector.h"
#include <exception>

using namespace mnsx::skuld;

void SkuldVision::setConfig(const Config& config) {
    config_ = config;
}

InspectResult SkuldVision::process(const cv::Mat &inputImage, uint64_t contextTag) {
    InspectResult result;
    result.contextTag = contextTag;

    // 拦截空白图片
    if (inputImage.empty()) {
        result.passed = false;
        result.status = SkuldStatus::ERR_EMPTY_INPUT;
        return result;
    }

    // 启动高精度计时器
    int64 startTick = cv::getTickCount();

    try {
        // 预处理
        cv::Mat gray = Inspector::preprocess(inputImage);

        // 二值化
        cv::Mat binary = Inspector::segment(gray);

        // 形态学优化
        cv::Mat optimized = binary;
        if (config_.enableMorphology) {
            optimized = Inspector::morphologicalOptimize(binary);
        }

        // 特征提取
        std::vector<cv::Rect> anomalies = Inspector::extractAnomalies(optimized, config_.minAnomalyArea);

        result.anomalyCount = anomalies.size();
        result.anomalyRects = anomalies;

        if (result.anomalyCount > 0) {
            result.passed = false;
            // TODO 设置不同的异常
            result.anomalyTypes.assign(result.anomalyCount, AnomalyType::BLOB);
        } else {
            result.passed = true;
        }

        result.status = SkuldStatus::SUCCESS;

    } catch (const cv::Exception& e) {
        result.passed = false;
        result.status = SkuldStatus::ERR_ALGORITHM_EXCEPTION;
    } catch (const std::exception& e) {
        result.passed = false;
        result.status = SkuldStatus::ERR_ALGORITHM_EXCEPTION;
    } catch (...) {
        result.passed = false;
        result.status = SkuldStatus::ERR_ALGORITHM_EXCEPTION;
    }

    int64 endTick = cv::getTickCount();
    double freq = cv::getTickFrequency();
    result.processTimeMs = ((endTick - startTick) / freq) * 1000.0;

    return result;
}