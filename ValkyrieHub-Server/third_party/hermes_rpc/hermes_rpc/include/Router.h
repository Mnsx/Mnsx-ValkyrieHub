/** 
 * @file Router.h
 * @author Mnsx_x <xx1527030652@gmail.com>
 * @date 2026/4/16
 * @description 解析消息中的body，返回对应的函数
 */
#ifndef MNSX_HERMESRPC_ROUTER_H
#define MNSX_HERMESRPC_ROUTER_H

#include <functional>
#include <unordered_map>
#include <string>

#include "DataStream.h"

namespace mnsx {
    namespace hermes {

        // 处理远程调用的函数
        using RpcHandler = std::function<std::string(DataStream& args)>;

        class Router {
        public:
            Router() = default;
            ~Router() = default;

            Router(const Router&) = delete;
            Router& operator=(const Router&) = delete;

            void registerMethod(const std::string method_name, RpcHandler handler);

            std::string route(const std::string& requestBody);

            bool hasMethod(const std::string& method_name) const;

        private:
            std::unordered_map<std::string, RpcHandler> handlers_;
        };

    }
}

#endif //MNSX_HERMESRPC_ROUTER_H