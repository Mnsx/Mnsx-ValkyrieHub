/** 
 * @file RpcServer.h
 * @author Mnsx_x <xx1527030652@gmail.com>
 * @date 2026/4/16
 * @description 
 */
#ifndef MNSX_HERMESRPC_RPCSERVER_H
#define MNSX_HERMESRPC_RPCSERVER_H

#include <memory>
#include "Router.h"

#include "TcpServer.h"
#include "reactor/EventLoop.h"

namespace mnsx {
    namespace hermes {

        class RpcServer {
        public:
            explicit RpcServer(uint16_t port);
            ~RpcServer() = default;

            RpcServer(const RpcServer&) = delete;
            RpcServer& operator=(const RpcServer&) = delete;

            Router& getRouter() { return router_; }

            void start();

        private:
            void onMessageCallback(const std::shared_ptr<achilles::TcpConnection>&, achilles::ByteBuffer*);

            Router router_;

            achilles::EventLoop loop_;

            std::unique_ptr<achilles::TcpServer> tcp_server_;
        };

    }
}

#endif //MNSX_HERMESRPC_RPCSERVER_H