/** 
 * @file HubServer.h
 * @author Mnsx_x <xx1527030652@gmail.com>
 * @date 2026/4/21
 * @description 
 */
#ifndef MNSX_VALKYRIEHUB_SERVER_HUBSERVER_H
#define MNSX_VALKYRIEHUB_SERVER_HUBSERVER_H

#include "Logger.h"
#include "../utils/MySQLUtils.h"
#include "KrakenPool.h"
#include "QtMethodDispatcher.h"
#include "RpcServer.h"
#include "nlohmann/json.hpp"

using namespace mnsx::hermes;
using namespace mnsx::kraken;

using json = nlohmann::json;

namespace mnsx {
    namespace valkyrie {
        constexpr static const int CLUSTER_PORT = 7777;
        constexpr static const int QT_PORT = 8888;

        class HubServer {
        public:
            explicit HubServer();
            ~HubServer();
        private:
            void startQtSocket();

            std::shared_ptr<RpcServer> rpc_server_; // 与集群通信的rpc服务
            std::shared_ptr<KrakenPool> pool_; // 处理任务的线程池

            // Qt
            std::thread qt_thread_; // 与Qt通信的线程
            int qt_server_fd_; // 与Qt通信的Socket
            int qt_client_fd_; // 连接服务端的客户端的Socket
            std::shared_ptr<QtMethodDispatcher> qt_dispatcher_; // Qt的方法路由
        };

    }
}

#endif //MNSX_VALKYRIEHUB_SERVER_HUBSERVER_H