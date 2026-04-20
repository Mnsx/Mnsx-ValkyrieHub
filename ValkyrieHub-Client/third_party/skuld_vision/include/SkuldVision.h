/** 
 * @file SkuldVision.h
 * @author Mnsx_x <xx1527030652@gmail.com>
 * @date 2026/4/15
 * @description 视觉引擎总控，负责串联算子并执行自动化检测流水线
 */
#ifndef MNSX_SKULDVISION_SKULDVISION_H
#define MNSX_SKULDVISION_SKULDVISION_H

#include <opencv2/core.hpp>

#include "InspectResult.h"

namespace mnsx {
    namespace skuld {

        class SkuldVision {
        public:
            // 服务的配置
            struct Config {
                double minAnomalyArea = 50.0; // 异常区块的最小面积阈值
                bool enableMorphology = true; // 是否开启形态学优化

                Config() : minAnomalyArea(50.0), enableMorphology(true) {}
            };

            /**
             * 构造函数
             * @param config
             * @return
             */
            explicit SkuldVision(const Config& config = Config()) : config_(config) {}

            /**
             * 析构函数
             * @return
             */
            ~SkuldVision() = default;

            /**
             * 图像处理函数
             * @param inputImage
             * @param contextTag
             * @return
             */
            InspectResult process(const cv::Mat& inputImage, uint64_t contextTag = 0);

            /**
             * config_ Setter
             * @param config
             */
            void setConfig(const Config& config);

        private:
            Config config_; // 配置信息
        };

    }
}

#endif //MNSX_SKULDVISION_SKULDVISION_H