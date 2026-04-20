/** 
 * @file DbLogHandler.cpp
 * @author Mnsx_x <xx1527030652@gmail.com>
 * @date 2026/4/20
 */
#include "handler/DbLogHandler.h"

using namespace mnsx::argus;

void DbLogHandler::handle(const LogEvent &event) {

    // 不记录Debug的数据
    if (event.level == LogLevel::DEBUG) {
        return;
    }

    // 日志内容
    std::string log_line_content = formatEvent(event);

    // 获取关键数据
    int node_id = event.node_number;
    int level = static_cast<int>(event.level);

    // 拼装SQL
    std::string sql = "INSERT INTO system_log (node_id, log_level, content) VALUES (";
    sql += std::to_string(event.node_number);
    sql += ", " + std::to_string(level);
    sql += ", '" + log_line_content + "');";

    MySQLUtil::getInstance().execute(sql);
}