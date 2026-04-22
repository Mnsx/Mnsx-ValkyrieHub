/** 
 * @file HubClient.cpp
 * @author Mnsx_x <xx1527030652@gmail.com>
 * @date 2026/4/22
 * @description 
 */
#include "core/HubClient.h"
#include "reactor/EventLoop.h"
#include "RpcClient.h"

using namespace mnsx::valkyrie;
using namespace mnsx::achilles;
using namespace mnsx::hermes;

HubClient::HubClient() : operator_(nullptr) {
    // 准备EventLoop
    EventLoop loop;
    // 组装客户端
    std::shared_ptr<RpcClient> client = std::make_shared<RpcClient>(&loop, SERVER_IP, SERVER_PORT);
    // 独立线程启动achilles
    std::thread io_thread([&loop]() {
        loop.loop();
    });
    // 客户端连接
    client->connect();
    // 启动图片处理器
    operator_ = CameraOperator(client);
    // TODO 传感器触发
    DataStream res = operator_.process();
    // io线程阻塞
    io_thread.join();
}
