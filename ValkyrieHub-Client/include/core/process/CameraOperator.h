/** 
 * @file CameraOperator.h
 * @author Mnsx_x <xx1527030652@gmail.com>
 * @date 2026/4/22
 * @description 
 */
#ifndef MNSX_VALKYRIEHUB_CAMERAOPERATOR_H
#define MNSX_VALKYRIEHUB_CAMERAOPERATOR_H

#include "DataStream.h"
#include <opencv2/opencv.hpp>

#include "InspectResult.h"
#include "KrakenPool.h"
#include "RpcClient.h"
#include "SkuldVision.h"

using namespace mnsx::kraken;
using namespace mnsx::skuld;
using namespace mnsx::hermes;

namespace mnsx {
    namespace valkyrie {

        class CameraOperator {
        public:
            CameraOperator(std::shared_ptr<RpcClient> rpc_client) : pool_(std::make_shared<KrakenPool>()),
                skuld_vision_(SkuldVision()),
                rpc_client_(std::move(rpc_client)){}

            ~CameraOperator() = default;
            // 进行图片处理
            DataStream process();
        private:
            DataStream imageAnalysis(cv::Mat mat);

            DataStream sendBadImageData(cv::Mat mat, InspectResult& res);

            std::shared_ptr<KrakenPool> pool_; // 图片分析线程池
            SkuldVision skuld_vision_;
            std::shared_ptr<RpcClient> rpc_client_;
        };
    }
}

#endif //MNSX_VALKYRIEHUB_CAMERAOPERATOR_H