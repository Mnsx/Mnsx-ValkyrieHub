/**
 * @file RpcServer.cpp
 * @author Mnsx_x <xx1527030652@gmail.com>
 * @date 2026/4/16
 */
#include "RpcServer.h"

#include "protocol/rpc/RpcCodec.h"
#include "reactor/EventLoop.h"
#include "protocol/rpc/RpcMessage.h"

using namespace mnsx::hermes;

RpcServer::RpcServer(uint16_t port) {
    tcp_server_ = std::unique_ptr<achilles::TcpServer>(new achilles::TcpServer(&loop_, 8080));
    tcp_server_->setMessageCallback([this](const std::shared_ptr<achilles::TcpConnection>& conn, achilles::ByteBuffer* buffer) {
        onMessageCallback(conn, buffer);
    });

    LOG_INFO << "[Hermes Server] Initializing RPC neural hub on port " << port << "...";
}

void RpcServer::start() {
    LOG_INFO << "[Hermes Server] RPC Server ignited. Awaiting connections...";
    tcp_server_->start();
    loop_.loop();
}

void RpcServer::onMessageCallback(const std::shared_ptr<achilles::TcpConnection> & conn, achilles::ByteBuffer * buffer) {
    // 解析数据
    achilles::RpcMessage req;
    achilles::RpcCodec::decode(buffer, req);
    // 处理心跳包
    if (req.msg_type == achilles::RpcMessageType::HEARTBEAT_PING) {
        achilles::RpcMessage pongMsg;
        pongMsg.version = req.version;
        pongMsg.msg_type = achilles::RpcMessageType::HEARTBEAT_PONG;
        pongMsg.request_id = req.request_id;

        achilles::ByteBuffer outBuffer;
        achilles::RpcCodec::encode(pongMsg, &outBuffer);
        conn->send(outBuffer.retrieveAllAsString());
        return;
    }

    // 业务处理
    if (req.msg_type == achilles::RpcMessageType::REQUEST) {
        achilles::RpcMessage respMsg;
        respMsg.version = req.version;
        respMsg.msg_type = achilles::RpcMessageType::RESPONSE;
        respMsg.request_id = req.request_id;

        try {
            respMsg.body = router_.route(req.body);
        } catch (const std::invalid_argument& e) {
            LOG_ERROR << "[Hermes Server] 路由失败! "
                      << ", RequestID: " << req.request_id
                      << ", 报错: " << e.what();
            respMsg.body = "RPC_ERROR: METHOD_NOT_FOUND";
        } catch (const std::out_of_range& e) {
            LOG_ERROR << "[Hermes Server] 数据流解析越界! "
                      << ", 报错: " << e.what();
            respMsg.body = "RPC_ERROR: DATASTREAM_UNDERFLOW";
        } catch (const std::exception& e) {
            LOG_ERROR << "[Hermes Server] 业务层致命异常! 报错: " << e.what();
            respMsg.body = std::string("RPC_ERROR: EXCEPTION_") + e.what();
        }

        achilles::ByteBuffer outBuffer;
        achilles::RpcCodec::encode(respMsg, &outBuffer);
        conn->send(outBuffer.retrieveAllAsString());
        return;
    }
}
