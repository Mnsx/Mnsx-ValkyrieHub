/** 
 * @file HubServer.cpp
 * @author Mnsx_x <xx1527030652@gmail.com>
 * @date 2026/4/21
 */
#include "core/HubServer.h"
#include "KrakenPool.h"
#include "RpcServer.h"

using namespace mnsx::valkyrie;
using namespace mnsx::kraken;
using namespace mnsx::hermes;


bool readExact(int fd, char* buffer, int length) {
    int bytesRead = 0;
    while (bytesRead < length) {
        int ret = recv(fd, buffer + bytesRead, length - bytesRead, 0);
        if (ret < 0) {
            return false;
        } else if (ret == 0) {
            return false;
        }
        bytesRead += ret;
    }
    return true;
}

HubServer::HubServer() : pool_(std::make_shared<KrakenPool>()),
    qt_dispatcher_(std::make_shared<QtMethodDispatcher>()) {
    // 日志测试
    LOG_INFO << "ValkyrieHub Server 启动";

    // 建立与Qt的Socket通信
    qt_thread_ = std::thread(&HubServer::startQtSocket, this);

    // 启动HermesRpc服务端
    rpc_server_ = std::make_shared<RpcServer>(CLUSTER_PORT);
    // 注册路由方法
    // 启动服务
    rpc_server_->start();
}

HubServer::~HubServer() {
    if (qt_thread_.joinable()) {
        qt_thread_.join();
    }
}

void HubServer::startQtSocket() {
    // 创建TcpSocket
    qt_server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (qt_server_fd_ < 0) {
        LOG_ERROR << "创建QtSocket连接失败";
        return;
    }

    // 启动端口复用
    int opt = 1;
    setsockopt(qt_server_fd_, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));

    // 绑定Ip和端口
    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(QT_PORT);

    if (bind(qt_server_fd_, reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr)) < 0) {
        LOG_WARN << "端口绑定失败" << QT_PORT << " " << std::strerror(errno);
        return;
    }

    // 开始监听
    if (listen(qt_server_fd_, 1) < 0) {
        LOG_WARN << "监听失败";
        return;
    }

    LOG_DEBUG << "ValkyrieHub Qt服务端启动";
    LOG_DEBUG << "等待 Qt客户端连接";

    while (true) {
        // 阻塞等待客户端连接
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        qt_client_fd_ = accept(qt_server_fd_, reinterpret_cast<struct sockaddr *>(&client_addr), &client_addr_len);
        if (qt_client_fd_ < 0) {
            LOG_ERROR << "接收连接失败";
            return ;
        }
        // 将fd保存到分发器中
        qt_dispatcher_->setQtFd(qt_client_fd_);

        LOG_DEBUG << "Qt客户端连接成功";

        // 循环接收数据
        while (true) {
            // 读取4字节包头
            uint32_t net_len = 0;
            if (!readExact(qt_client_fd_, reinterpret_cast<char *>(&net_len), 4)) {
                break; // 读取失败退出循环
            }
            // 将字节数据转换为大端
            uint32_t body_len = ntohl(net_len);
            // 排除垃圾数据
            constexpr uint32_t MAX_PACKET_SIZE = 10 * 1024 * 1024;
            if (body_len == 0 || body_len > MAX_PACKET_SIZE) {
                LOG_ERROR << "收到异常的数据包长度: " << body_len << "，强制断开连接防范溢出！";
                break;
            }
            // 根据字节数据长度获取Json数据
            std::vector<char> json_buffer(body_len + 1, 0);
            if (!readExact(qt_client_fd_, json_buffer.data(), body_len)) {
                break;
            }
            std::string json_str(json_buffer.data(), body_len);
            // 分发任务
            qt_dispatcher_->dispatchQtTask(json_str);
        }
    }
}

