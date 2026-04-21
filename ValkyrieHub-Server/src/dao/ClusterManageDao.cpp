/** 
 * @file ClusterManageDao.cpp
 * @author Mnsx_x <xx1527030652@gmail.com>
 * @date 2026/4/21
 * @description 
 */
#include "dao/ClusterManageDao.h"

#include "Logger.h"
#include "utils/MySQLUtils.h"

using namespace mnsx::valkyrie;

Json ClusterManageDao::selectAll() {
    std::string sql = "SELECT * FROM device_node WHERE is_deleted = 0;";
    MySQLResult result = MySQLUtil::getInstance().query(sql);
    if (result == nullptr) {
        // 应该有返回值却没拿到，说明提取失败
        LOG_ERROR << "获取结果集失败: " << MySQLUtil::getInstance().getError();
        return Json::array();
    }

    // 获取表头元数据
    int num_fields = mysql_num_fields(result.get());
    MYSQL_FIELD* fields = mysql_fetch_fields(result.get());

    Json result_array = Json::array();
    MYSQL_ROW row;

    // 4. 逐行解析数据
    while ((row = mysql_fetch_row(result.get())) != nullptr) {
        Json row_json = Json::object();

        // 遍历当前行的每一列
        for (int i = 0; i < num_fields; i++) {
            std::string column_name = fields[i].name;

            if (row[i] != nullptr) {
                row_json[column_name] = row[i];
            } else {
                row_json[column_name] = nullptr; // 映射为 JSON 的 null
            }
        }
        result_array.push_back(row_json);
    }

    return result_array;
}

Json ClusterManageDao::deleteClusterByMac(const std::string &mac) {
    std::string sql = "UPDATE device_node SET is_deleted = 1, update_time = NOW() WHERE mac_address ='"
    + MySQLUtil::getInstance().escapeString(mac) + "';";
    MySQLUtil::getInstance().execute(sql);
    return Json::array();
}
