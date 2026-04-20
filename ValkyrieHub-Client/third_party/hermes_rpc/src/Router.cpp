/**
 * @file Router.cpp
 * @author Mnsx_x <xx1527030652@gmail.com>
 * @date 2026/4/16
 */
#include "../include/Router.h"
#include "../third_party/argus_log/include/Logger.h"

using namespace mnsx::hermes;

void Router::registerMethod(const std::string method_name, RpcHandler handler) {
    handlers_[method_name] = std::move(handler);

    LOG_INFO << "[Hermes Router] 路由注册成功: " << method_name;
}

std::string Router::route(const std::string &requestBody) {
    // 将接收的二进制转换为数据流
    DataStream stream(requestBody);
    // 去除方法名
    std::string method_name;
    stream >> method_name;

    // 找寻业务函数
    auto it = handlers_.find(method_name);
    if (it == handlers_.end()) {
        throw std::invalid_argument("RPC Method not found: " + method_name);
    }

    return it->second(stream);
}

bool Router::hasMethod(const std::string &method_name) const {
    return handlers_.find(method_name) != handlers_.end();
}
