/** 
 * @file ClusterManageService.h
 * @author Mnsx_x <xx1527030652@gmail.com>
 * @date 2026/4/21
 * @description 
 */
#ifndef MNSX_VALKYRIEHUB_SERVER_CLUSTERMANAGESERVICE_H
#define MNSX_VALKYRIEHUB_SERVER_CLUSTERMANAGESERVICE_H
#include "nlohmann/json.hpp"

using Json = nlohmann::json;

namespace mnsx {
    namespace valkyrie {

        class ClusterManageService {
        public:
            static ClusterManageService& getInstance() {
                static ClusterManageService instance;
                return instance;
            }

            Json getAllCluster();

            Json removeClusterByMac(const std::string& mac);

        private:
            ClusterManageService() = default;
            ~ClusterManageService() = default;
        };

    }
}

#endif //MNSX_VALKYRIEHUB_SERVER_CLUSTERMANAGESERVICE_H