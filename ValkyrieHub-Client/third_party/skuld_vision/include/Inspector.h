#pragma once
#include <opencv2/opencv.hpp>
#include <onnxruntime_cxx_api.h>
#include <vector>
#include <string>
#include <memory>

namespace mnsx {
    namespace skuld {

        class Inspector {
        public:
            static bool initAIEngine(const std::string& onnxPath);
            static void extractAnomaliesYOLO(const cv::Mat& src,
                                             std::vector<cv::Rect>& outRects,
                                             std::vector<int>& outClassIds,
                                             float confThreshold = 0.5f);

        private:
            static bool isAiLoaded_;

            // 使用 C++11 标准的 unique_ptr
            static std::unique_ptr<Ort::Env> env_;
            static std::unique_ptr<Ort::Session> session_;
            static Ort::MemoryInfo memoryInfo_;
        };

    } // namespace skuld
} // namespace mnsx