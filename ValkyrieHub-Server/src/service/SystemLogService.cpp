/** 
 * @file SystemLogService.cpp
 * @author Mnsx_x <xx1527030652@gmail.com>
 * @date 2026/4/21
 * @description 
 */
#include "service/SystemLogService.h"

#include "dao/SystemLogDao.h"

using namespace mnsx::valkyrie;

Json SystemLogService::getSystemLog() {

    Json type = {{"type", "LOG"}};
    Json res = SystemLogDao::getInstance().selectAll();
    res.insert(res.begin(), type);
    return res;
}
