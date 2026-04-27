/** 
 * @file HubServer.cpp
 * @author Mnsx_x <xx1527030652@gmail.com>
 * @date 2026/4/21
 */
#include "core/HubServer.h"
#include "KrakenPool.h"
#include "RpcServer.h"
#include "service/ClusterManageService.h"
#include <fstream>
#include <opencv2/opencv.hpp>
#include "service/DefectRecordService.h"
#include "utils/Base64Utils.h"

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
    // 注册客户端注册
    rpc_server_->getRouter().registerMethod("ClusterManage.registerCluster", [this](DataStream& data) {
        std::vector<std::string> args = getArgs(data);
        registerCluster(args);
        return "";
    });
    rpc_server_->getRouter().registerMethod("DefectApproval.postAnomalyInfo", [this](DataStream& data) {
        std::vector<std::string> args = getImageArgs(data);
        pool_->enqueue([this, args]() {
            doImageTask(args);
        });
        return "";
    });
    // 启动服务
    rpc_server_->start();
}

HubServer::~HubServer() {
    if (qt_thread_.joinable()) {
        qt_thread_.join();
    }
}

void HubServer::registerCluster(std::vector<std::string> args) {
    Json res = ClusterManageService::getInstance().registerCluster(args[0], args[1], args[2]);
    qt_dispatcher_->sendToQtClient(qt_client_fd_, res);
}

std::vector<std::string> HubServer::getImageArgs(DataStream &data_stream) {

    std::string reportData;
    std::string image_data;
    data_stream >> reportData >> image_data;
    std::vector<std::string> args;
    std::string arg;
    std::stringstream ss(reportData);
    while (std::getline(ss, arg, '|')) {
        args.push_back(arg);
    }
    args.push_back(image_data);
    return args;
}

std::vector<std::string> HubServer::getArgs(DataStream &data_stream) {

    std::string reportData;
    data_stream >> reportData;
    std::vector<std::string> args;
    std::string arg;
    std::stringstream ss(reportData);
    while (std::getline(ss, arg, '|')) {
        args.push_back(arg);
    }
    return args;
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

void HubServer::doImageTask(std::vector<std::string> args) {
    // 处理参数数据
    std::string mac_addr = args[0];
    std::string anomaly_count = args[1];
    std::string anomaly_rects = args[2];
    std::string anomaly_type = args[3];
    std::string image_origin = args[4];
    // 获取时间戳
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::stringstream time_ss;
    time_ss << std::put_time(std::localtime(&now_c), "%Y%m%d_%H%M%S");
    std::string timestamp = time_ss.str();
    // 将mac地址转换
    std::string safe_mac = mac_addr;
    std::replace(safe_mac.begin(), safe_mac.end(), ':', '_');

    // 将图片数据存储到硬盘位置
    std::string save_dir = "./valkyrie_data/defect_images/";
    std::string cmd = "mkdir -p " + save_dir;
    int ret = std::system(cmd.c_str());
    if (ret != 0) {
        LOG_WARN << "[Warning] 确保存储目录状态 (" << ret << ")";
    }
    // 拼接名称
    std::string file_path = save_dir + safe_mac + "_" + timestamp + ".jpg";
    // 存储
    std::ofstream outfile(file_path, std::ios::binary);
    if (outfile.is_open()) {
        outfile.write(image_origin.data(), image_origin.size());
        outfile.close();
        LOG_INFO << "图片保存成功" + file_path;
    } else {
        LOG_ERROR << "保存图片错误";
    }

    // 制作缩略图存储到硬盘位置
    // 将字符串图片数据转换为Mat
    std::string base64_str = "";
    std::vector<uchar> img_buf(image_origin.begin(), image_origin.end());
    cv::Mat raw_img = cv::imdecode(img_buf, cv::IMREAD_COLOR);
    std::string thumb_path;
    std::vector<int> thumb_params;
    if (!raw_img.empty()) {
        // 设定缩略图的极限尺寸
        int target_width = 320;
        int target_height = (raw_img.rows * target_width) / raw_img.cols;

        cv::Mat thumb_img;
        //使用cv::INTER_AREA算法
        cv::resize(raw_img, thumb_img, cv::Size(target_width, target_height), 0, 0, cv::INTER_AREA);

        // 准备缩略图存储目录
        std::string thumb_dir = "./valkyrie_data/defect_thumbs/";
        std::string cmd_thumb = "mkdir -p " + thumb_dir;
        std::system(cmd_thumb.c_str());

        // 拼接缩略图路径
        thumb_path = thumb_dir + safe_mac + "_" + timestamp + "_thumb.jpg";

        // 压缩并落盘
        thumb_params = {cv::IMWRITE_JPEG_QUALITY, 75};

        // 直接imwrite保存到硬盘
        if (cv::imwrite(thumb_path, thumb_img, thumb_params)) {
            LOG_INFO << "缩略图保存成功" + file_path;

            std::vector<uchar> thumb_buf;
            cv::imencode(".jpg", thumb_img, thumb_buf, thumb_params);
            base64_str = base64_encode(thumb_buf.data(), thumb_buf.size());
        } else {
            LOG_ERROR << "缩略图保存错误";
        }
    } else {
            LOG_ERROR << "OpenCV制作缩略图算法错误";
    }

    // 将图片数据存储到数据库中
    // 准备数据库存储数据参数
    std::vector<std::string> param_db;
    param_db.push_back(mac_addr);
    param_db.push_back(anomaly_type);
    param_db.push_back(file_path);
    param_db.push_back(thumb_path);
    param_db.push_back(timestamp);
    bool addSuccess = DefectRecordService::getInstance().addNewDefectRecord(param_db);
    if (addSuccess == true) {
        // 将缩略图和瑕疵数据发送到Qt端
        std::vector<uchar> thumb_buf;
        try {
            // 1. 提前校验输入
            if (img_buf.empty()) {
                LOG_ERROR << "输入图像为空，跳过编码";
                return;
            }

            // 2. 执行编码
            bool success = cv::imencode(".jpg", img_buf, thumb_buf, thumb_params);
            if (!success) {
                LOG_WARN << "imencode 返回 false，编码失败但未抛出异常";
            }
        }
        // 3. 改为按常量引用捕获，并优先捕获 cv::Exception
        catch (const cv::Exception& e) {
            LOG_ERROR << "OpenCV 异常: " << e.what();
        }
        catch (const std::exception& e) {
            LOG_ERROR << "标准异常: " << e.what();
        }
        catch (...) {
            LOG_ERROR << "未知崩溃";
        }
        Json result_array = Json::array();
        Json json_obj = {
            {"mac", mac_addr}, {"count", anomaly_count}, {"rects", anomaly_rects},
            {"rtype", anomaly_type}, {"path", file_path}, {"img", base64_str}, {"time", timestamp}
        };
        result_array.push_back(json_obj);
        Json type = {{"type", "SIGNAL"}};
        result_array.insert(result_array.begin(), type);
        qt_dispatcher_->sendToQtClient(qt_client_fd_, result_array);
    }
}

