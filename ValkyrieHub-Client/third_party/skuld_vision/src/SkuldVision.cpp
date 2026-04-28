/**
* @file SkuldVision.cpp
 * @author Mnsx_x <xx1527030652@gmail.com>
 * @date 2026/4/15
 */
#include "SkuldVision.h"
#include "Inspector.h"
#include <exception>
#include <iostream>
#include <random>

using namespace mnsx::skuld;

void SkuldVision::setConfig(const Config& config) {
    config_ = config;
}

InspectResult SkuldVision::process(const cv::Mat &inputImage, uint64_t contextTag) {
    InspectResult result;
    result.contextTag = contextTag;
    result.passed = true; // 默认通过

    if (inputImage.empty()) {
        result.passed = false;
        result.status = SkuldStatus::ERR_EMPTY_INPUT;
        return result;
    }

    int64 startTick = cv::getTickCount();

    try {
        std::vector<cv::Rect> anomalies;
        std::vector<int> anomalyClassIds;

        Inspector::extractAnomaliesYOLO(inputImage, anomalies, anomalyClassIds, 0.25f);

        result.anomalyCount = anomalies.size();
        result.anomalyRects = anomalies;

        if (result.anomalyCount > 0) {
            result.passed = false;
            // TODO 设置不同的异常
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(0, 4);
            int random_num = dis(gen);
            result.anomalyTypes.assign(result.anomalyCount, static_cast<AnomalyType>(random_num));
        } else {
            result.passed = true;
        }

        result.status = SkuldStatus::SUCCESS;

    } catch (const std::exception& e) {
        result.passed = false;
        result.status = SkuldStatus::ERR_ALGORITHM_EXCEPTION;
        std::cerr << "💥 Skuld 引擎异常: " << e.what() << std::endl;
    } catch (...) {
        result.passed = false;
        result.status = SkuldStatus::ERR_ALGORITHM_EXCEPTION;
    }

    int64 endTick = cv::getTickCount();
    result.processTimeMs = ((endTick - startTick) / cv::getTickFrequency()) * 1000.0;

    return result;
}