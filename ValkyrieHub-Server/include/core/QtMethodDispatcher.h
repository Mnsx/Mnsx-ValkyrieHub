/** 
 * @file QtMethodDIspatcher.h
 * @author Mnsx_x <xx1527030652@gmail.com>
 * @date 2026/4/21
 * @description 
 */
#ifndef MNSX_VALKYRIEHUB_SERVER_QTMETHODDISPATCHER_H
#define MNSX_VALKYRIEHUB_SERVER_QTMETHODDISPATCHER_H

#include "KrakenPool.h"
#include "nlohmann/json.hpp"

#include <unordered_map>

using namespace mnsx::kraken;

using Json = nlohmann::json;

namespace mnsx {
    namespace valkyrie {

        class QtMethodDispatcher {
        public:
            explicit QtMethodDispatcher();

            void setQtFd(int qt_fd) { qt_fd_ = qt_fd; }

            void dispatchQtTask(const std::string& json_str);
            bool sendToQtClient(int client_fd, const Json& response_data);
        private:
            std::shared_ptr<KrakenPool> qt_pool_; // 专门处理qt的线程池
            std::unordered_map<std::string, std::function<Json(const Json&)>> router_; // 存放转发的消息
            int qt_fd_;
        };

    }
}

#endif //MNSX_VALKYRIEHUB_SERVER_QTMETHODDISPATCHER_H