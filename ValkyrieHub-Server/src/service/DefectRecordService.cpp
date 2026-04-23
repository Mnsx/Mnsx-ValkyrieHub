/** 
 * @file DefectRecordService.cpp
 * @author Mnsx_x <xx1527030652@gmail.com>
 * @date 2026/4/23
 * @description 
 */
#include "service/DefectRecordService.h"

#include "dao/DefectRecordDao.h"

using namespace mnsx::valkyrie;

bool DefectRecordService::addNewDefectRecord(std::vector<std::string> params) {
    return DefectRecordDao::getInstance().insertDefectRecord(params);
}

Json DefectRecordService::getAllDetectRecord() {
    return Json();
}
