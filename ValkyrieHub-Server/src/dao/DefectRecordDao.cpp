/** 
 * @file DefectRecordDao.cpp
 * @author Mnsx_x <xx1527030652@gmail.com>
 * @date 2026/4/23
 * @description 
 */
#include "dao/DefectRecordDao.h"

#include "utils/MySQLUtils.h"

using namespace mnsx::valkyrie;

bool DefectRecordDao::insertDefectRecord(std::vector<std::string> params) {
    std::string sql = "INSERT INTO defect_record (node_id, defect_type, image_path, thumbnail_path, capture_time) VALUES ('" +
        params[0] + "', '" +
            params[1] + "', '" +
                params[2] + "', '" +
                    params[3] + "', '" +
                        params[4] + "');";

    MySQLUtil::getInstance().execute(sql);
    std::string rs = MySQLUtil::getInstance().getError();
    return true;
}

Json DefectRecordDao::selectAll() {

}
