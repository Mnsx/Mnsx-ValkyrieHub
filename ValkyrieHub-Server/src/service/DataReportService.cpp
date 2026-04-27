/** 
 * @file DataReportService.cpp
 * @author Mnsx_x <xx1527030652@gmail.com>
 * @date 2026/4/24
 * @description 
 */
#include "service/DataReportService.h"

#include "utils/MySQLUtils.h"

using namespace mnsx::valkyrie;

Json DataReportService::getRealtimeYield() {
    // 获取产品总数
    std::string sql = "SELECT count FROM total_productions WHERE id = 1;";
    MySQLResult res = MySQLUtil::getInstance().query(sql);
    MYSQL_ROW row = mysql_fetch_row(res.get());
    int total_count = std::stoi(row[0]);
    // 统计异常产品总数
        sql = "SELECT COUNT(*) FROM defect_record WHERE review_status != 2;";
    res = MySQLUtil::getInstance().query(sql);
    row = mysql_fetch_row(res.get());
    int total_detect_count = std::stoi(row[0]);
    // 求出实时的异常比
    double proportion = (total_detect_count / 1.0) / total_count;
    // 返回数据
    Json result_array = Json::array();
    result_array.push_back({{"res", proportion}});
    Json type = {{"type", "REALTIMEYIELD"}};
    result_array.insert(result_array.begin(), type);
    return result_array;
}

Json DataReportService::falsePostitiveRateWeekly() {
    std::string sql = R"(
    SELECT
        LEFT(capture_time, 8) AS report_date,
        (SUM(IF(review_status = 2, 1, 0)) / COUNT(*)) * 100 AS fpr_rate
    FROM
        defect_record
    WHERE
        STR_TO_DATE(LEFT(capture_time, 8), '%Y%m%d') >= DATE_SUB(CURDATE(), INTERVAL 7 DAY)
    GROUP BY
        LEFT(capture_time, 8)
    ORDER BY
        report_date ASC;
    )";
    MySQLResult res = MySQLUtil::getInstance().query(sql);

    Json json_array = Json::array();
    if (res) {
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(res.get()))) {
            Json temp = Json::object();
            if (row[0] != nullptr) {
                std::string fullDate = row[0];
                temp["date"] = fullDate.substr(fullDate.length() - 4);
            }
            if (row[1] != nullptr) {
                temp["value"] = std::stod(row[1]);
            } else {
                temp["value"] = 0.0;
            }
            json_array.push_back(temp);
        }
        Json type = {{"type", "FALSERATE"}};
        json_array.insert(json_array.begin(), type);
    }
    return json_array;
}