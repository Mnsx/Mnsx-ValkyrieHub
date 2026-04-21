/** 
 * @file ClusterManageService.cpp
 * @author Mnsx_x <xx1527030652@gmail.com>
 * @date 2026/4/21
 * @description 
 */
#include "service/ClusterManageService.h"

#include "dao/ClusterManageDao.h"

using namespace mnsx::valkyrie;

Json ClusterManageService::getAllCluster() {
    Json type = {{"type", "DATA"}};
    Json res = ClusterManageDao::getInstance().selectAll();
    res.insert(res.begin(), type);
    return res;
}

Json ClusterManageService::removeClusterByMac(const std::string &mac) {
    ClusterManageDao::getInstance().deleteClusterByMac(mac);
    Json type = {{"type", "DATA"}};
    Json res = ClusterManageDao::getInstance().selectAll();
    res.insert(res.begin(), type);
    return res;
}
