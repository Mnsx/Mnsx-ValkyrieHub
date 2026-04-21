/** 
 * @file ClusterManageDao.h
 * @author Mnsx_x <xx1527030652@gmail.com>
 * @date 2026/4/21
 * @description 
 */
#ifndef MNSX_VALKYRIEHUB_SERVER_CLUSTERMANAGEDAO_H
#define MNSX_VALKYRIEHUB_SERVER_CLUSTERMANAGEDAO_H
#include "nlohmann/json.hpp"

using Json = nlohmann::json;

namespace mnsx {
    namespace valkyrie {

        class ClusterManageDao {
        public:
            static ClusterManageDao& getInstance() {
                static ClusterManageDao instance;
                return instance;
            }

            Json selectAll();

            Json deleteClusterByMac(const std::string & mac);

        private:
            ClusterManageDao() = default;
            ~ClusterManageDao() = default;
        };

    }
}

#endif //MNSX_VALKYRIEHUB_SERVER_CLUSTERMANAGEDAO_H