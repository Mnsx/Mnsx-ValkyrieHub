/** 
 * @file Types.h
 * @author Mnsx_x <xx1527030652@gmail.com>
 * @date 2026/4/15
 * @description 包含状态码，异常状态，和传输的消息
 */
#ifndef MNSX_SKULDVISION_TYPES_H
#define MNSX_SKULDVISION_TYPES_H

#include <vector>
#include <opencv2/core.hpp>

namespace mnsx {
    namespace skuld {
        /**
         * 状态码
         */
        enum class SkuldStatus {
            SUCCESS = 0,
            ERR_EMPTY_INPUT,
            ERR_DECODE_FAILED,
            ERR_ALGORITHM_EXCEPTION
        };

        enum class AnomalyType {
            NONE = 0, // 无异常
            BLOB, // 斑块/污点
            EDGE_BREAK, // 边缘断裂
            LINE_SCRATCH, // 线条状划痕
            SHAPE_MISMATCH // 形状轮廓不匹配
        };

        struct InspectResult {
            // 判定结论
            bool passed = true;
            SkuldStatus status = SkuldStatus::SUCCESS;
            // 异常特征
            int anomalyCount = 0; // 异常数量
            std::vector<cv::Rect> anomalyRects; // 异常边界框
            std::vector<AnomalyType> anomalyTypes; // 异常分类
            // 标识符
            uint64_t contextTag = 0;
            // 运算时间
            double processTimeMs;
        };
    }
}

#endif //MNSX_SKULDVISION_TYPES_H