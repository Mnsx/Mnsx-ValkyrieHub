/**
 * @file QtMethodDispatcher.cpp
 * @author Mnsx_x <xx1527030652@gmail.com>
 * @date 2026/4/21
 */
#include "core/QtMethodDispatcher.h"

#include <netinet/in.h>

#include "service/ClusterManageService.h"
#include "service/DataReportService.h"
#include "service/DefectRecordService.h"
#include "service/SystemLogService.h"
#include "utils/FileUtils.h"
#include "utils/Base64Utils.h"

using namespace mnsx::valkyrie;

QtMethodDispatcher::QtMethodDispatcher() : qt_pool_(std::make_shared<KrakenPool>()) {
    router_["SystemLog.getSystemLog"] = [this](const Json&) {
        return SystemLogService::getInstance().getSystemLog();
    };
    router_["ClusterManage.getAllCluster"] = [this](const Json&) {
        return ClusterManageService::getInstance().getAllCluster();
    };
    router_["ClusterManage.removeCluster"] = [this](const Json& data) {
        std::string mac = data["nodeMac"];
        return ClusterManageService::getInstance().removeClusterByMac(mac);
    };
    router_["DefectRecord.getAllDetectRecord"] = [this](const Json& data) {
        return DefectRecordService::getInstance().getAllDetectRecord();
    };
    router_["DetectRecord.getRecordFromPath"] = [this](const Json& data) {
        std::vector<unsigned char> img_bytes = readImageFromDisk(data["path"]);
        std::string base64_str = base64_encode(img_bytes.data(), img_bytes.size());
        Json result_array = Json::array();
        Json res = {{"img", base64_str}};
        result_array.push_back(res);
        Json type = {{"type", "IMAGE"}};
        result_array.insert(result_array.begin(), type);
        return result_array;
    };
    router_["DataReport.getRealtimeYield"] = [this](const Json& data) {
        return DataReportService::getInstance().getRealtimeYield();
    };
    router_["DataReport.falsePositiveRateWeekly"] = [this](const Json& data) {
        return DataReportService::getInstance().falsePostitiveRateWeekly();
    };
}

void QtMethodDispatcher::dispatchQtTask(const std::string &json_str) {
    // 解析JSON
    try {
        // 解析字符串为 JSON 对象
        Json j = Json::parse(json_str);

        std::string method_name = j["method"];

        std::string type = j["type"];
        // 如果不是方法，跳出
        if (type != "METHOD") {
            return;
        }

        // std::string method_name = j["method"];
        if (method_name == "") {
            return;
        }

        // 去路由表里找对应的处理函数
        auto it = router_.find(method_name);
        if (it != router_.end()) {
            // 交给线程池处理
            qt_pool_->enqueue([this, it, j]() {
                Json res = it->second(j);
                sendToQtClient(qt_fd_, res);
            });
        } else {
            LOG_WARN << "收到未知的路由请求类型: " << type;
        }
    } catch (const std::exception& e) {
        LOG_ERROR << "JSON 解析或路由分发失败: " << e.what() << "，原始数据: " << json_str;
    }
}

bool QtMethodDispatcher::sendToQtClient(int client_fd, const Json &response_data) {
    if (client_fd < 0) {
        LOG_WARN << "无效的客户端 FD，放弃发送";
        return false;
    }
    std::string json_str = response_data.dump();

    // 计算文本呢长度
    uint32_t body_len = json_str.size();
    uint32_t net_len = htonl(body_len);

    // 先发送4字节的消息长度
    ssize_t head_ret = send(client_fd, &net_len, 4, MSG_NOSIGNAL);
    if (head_ret <= 0) {
        LOG_ERROR << "发送包头失败，客户端可能已断开";
        return false;
    }
    // 发送消息体
    ssize_t body_ret = send(client_fd, json_str.c_str(), body_len, MSG_NOSIGNAL);
    if (body_ret <= 0) {
        LOG_ERROR << "发送报文体失败";
        return false;
    }

    LOG_DEBUG << "成功向 Qt 发送响应: " << json_str;
    return true;
}
