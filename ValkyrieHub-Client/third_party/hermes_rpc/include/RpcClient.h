/**
 * @file RpcClient.h
 * @author Mnsx_x
 * @description
 */
#ifndef MNSX_HERMESRPC_RPCCLIENT_H
#define MNSX_HERMESRPC_RPCCLIENT_H

#include "DataStream.h"
#include "reactor/EventLoop.h"
#include "net/TcpConnection.h"
#include "TcpClient.h"
#include "KrakenPool.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <functional>
#include <mutex>
#include <atomic>

namespace mnsx {
namespace hermes {

    // 异步回调函数的签名
    using RpcCallback = std::function<void(const std::string& responseBody)>;

    class RpcClient : public std::enable_shared_from_this<RpcClient> {
    public:
        RpcClient(achilles::EventLoop* loop, kraken::KrakenPool* kraken, const std::string& server_ip, uint16_t port);

        ~RpcClient() = default;

        // 禁用拷贝语义
        RpcClient(const RpcClient&) = delete;
        RpcClient& operator=(const RpcClient&) = delete;

        void connect();

        void call(const std::string& method_name, const std::string& serialized_args, RpcCallback callback);

    private:
        void onConnectionCallback(const std::shared_ptr<achilles::TcpConnection>& conn);
        void onMessageCallback(const std::shared_ptr<achilles::TcpConnection>& conn, achilles::ByteBuffer* buffer);

        achilles::EventLoop* loop_;
        mnsx::kraken::KrakenPool* kraken_;

        std::unique_ptr<achilles::TcpClient> tcp_client_;
        std::shared_ptr<achilles::TcpConnection> connection_;
        std::mutex conn_mutex_;

        std::atomic<uint64_t> request_id_generator_{0};
    };

} // namespace hermes
} // namespace mnsx

#endif // MNSX_HERMESRPC_RPCCLIENT_H