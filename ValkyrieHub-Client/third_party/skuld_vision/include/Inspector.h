/** 
 * @file Inspector.h
 * @author Mnsx_x <xx1527030652@gmail.com>
 * @date 2026/4/15
 * @description 视觉检验算子核心实现
 */
#ifndef MNSX_SKULDVISION_INSPECTOR_H
#define MNSX_SKULDVISION_INSPECTOR_H

#include <opencv2/core.hpp>

namespace mnsx {
    namespace skuld {

        class Inspector {
        public:
            /**
             * @brief 灰度化与去噪
             * @param src 输入的原始 BGR 图像
             * @return 降噪后的灰度图
             */
            static cv::Mat preprocess(const cv::Mat& src);

            /**
             * @brief 图像分割
             * @param gray 预处理后的灰度图
             * @return 二值图
             */
            static cv::Mat segment(const cv::Mat& gray);

            /**
             * @brief 特征提取
             * @param binary 二值图
             * @param minArea 最小面积阈值（过滤噪点）
             * @return 异常区域的矩形集合
             */
            static std::vector<cv::Rect> extractAnomalies(const cv::Mat& binary, double minArea = 50.0);

            /**
             * @brief 连接断裂的划痕或消除细小噪点
             */
            static cv::Mat morphologicalOptimize(const cv::Mat& binary);
        };

    }
}

#endif //MNSX_SKULDVISION_INSPECTOR_H