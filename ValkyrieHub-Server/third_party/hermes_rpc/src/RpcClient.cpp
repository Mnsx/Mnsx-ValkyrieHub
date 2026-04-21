/** 
 * @file RpcClient.cpp
 * @author Mnsx_x <xx1527030652@gmail.com>
 * @date 2026/4/16
 */
#include "RpcClient.h"

#include "protocol/rpc/RpcCodec.h"
#include "protocol/rpc/RpcMessage.h"

using namespace mnsx::hermes;

RpcClient::RpcClient(achilles::EventLoop* loop, const std::string& server_ip, uint16_t port) : loop_(loop) {
    achilles::InetAddress server_addr(port, server_ip);
    tcp_client_ = std::unique_ptr<achilles::TcpClient>(new achilles::TcpClient(loop_, server_addr));

    tcp_client_->setConnectionCallback([this](const std::shared_ptr<achilles::TcpConnection>& conn) {
        onConnectionCallback(conn);
    });
}

void RpcClient::connect() {
    LOG_INFO << "[Hermes Client] Initiating neural link to Valkyrie Hub...";
    tcp_client_->connect();
}

void RpcClient::onConnectionCallback(const std::shared_ptr<achilles::TcpConnection> &conn) {
    std::lock_guard<std::mutex> lock(conn_mutex_);
    connection_ = conn;
    LOG_INFO << "[Hermes Client] Link established! Kraken pool standing by.";
}

void RpcClient::call(const std::string &method_name, const std::string &serialized_args) {
    std::shared_ptr<achilles::TcpConnection> current_conn;
    {
        std::lock_guard<std::mutex> lock(conn_mutex_);
        current_conn = connection_;
    }

    if (!current_conn) {
        LOG_ERROR << "[Hermes Client] Async call failed! Network disconnected.";
        return;
    }

    uint64_t reqId = ++request_id_generator_;

    DataStream stream("");
    stream << method_name;
    std::string finalBody = stream.data() + serialized_args;

    achilles::RpcMessage reqMsg;
    reqMsg.version = 1;
    reqMsg.msg_type = achilles::RpcMessageType::REQUEST;
    reqMsg.request_id = reqId;
    reqMsg.body = finalBody;

    achilles::ByteBuffer outBuffer;
    achilles::RpcCodec::encode(reqMsg, &outBuffer);

    current_conn->send(outBuffer.retrieveAllAsString());
}