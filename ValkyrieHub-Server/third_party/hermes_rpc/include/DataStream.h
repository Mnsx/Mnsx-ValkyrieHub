/** 
 * @file DataStream.h
 * @author Mnsx_x <xx1527030652@gmail.com>
 * @date 2026/4/16
 * @description 用来作为RPC协议的消息体
 */
#ifndef MNSX_HERMESRPC_DATASTREAM_H
#define MNSX_HERMESRPC_DATASTREAM_H

#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <string>
#include <memory>

namespace mnsx {
    namespace hermes {

        class DataStream {
        public:
            /**
             * 构造函数
             */
            explicit DataStream(const std::string& str) : buffer_(str), read_pos_(0) {}
            /**
             * 获取数据流中的数据
             * @return
             */
            const std::string& data() {
                return buffer_;
            }
            /**
             * 重置流，并将数据注入
             * @param data
             */
            void reset(const std::string& data) {
                read_pos_ = 0;
                buffer_ = data;
            }

            template<typename T>
            typename std::enable_if<std::is_trivially_copyable<T>::value, DataStream&>::type
                operator<<(const T& value) {

                size_t old_size = buffer_.size();
                // 扩容
                buffer_.resize(old_size + sizeof(T));
                // 数据拷贝
                std::memcpy(&buffer_[old_size], &value, sizeof(T));
                return *this;
            }

            DataStream& operator<<(const std::string& value) {
                uint32_t len = static_cast<uint32_t>(value.size());
                // 先将数据传入
                *this << len;
                if (len > 0) {
                    *this << value;
                }
                return *this;
            }

            template <typename T>
            typename std::enable_if<std::is_trivially_copyable<T>::value, DataStream&>::type
                operator>>(T& value) {

                if (read_pos_ + sizeof(T) > buffer_.size()) {
                    throw std::out_of_range("[Hermes DataStream] Buffer underflow during basic type read.");
                }
                std::memcpy(&value, &buffer_[read_pos_], sizeof(T));
                read_pos_ += sizeof(T);
                return *this;
            }

            DataStream& operator>>(std::string& value) {
                uint32_t len;
                *this >> len; // 先读出长度

                if (read_pos_ + len > buffer_.size()) {
                    throw std::out_of_range("[Hermes DataStream] Buffer underflow during string read.");
                }
                value.assign(&buffer_[read_pos_], len);
                read_pos_ += len;
                return *this;
            }

        private:
            std::string buffer_; // 存放数据的容器
            int read_pos_; // 读指针
        };

    }
}

#endif //MNSX_HERMESRPC_DATASTREAM_H