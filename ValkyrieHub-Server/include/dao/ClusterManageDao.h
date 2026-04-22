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

            Json addCluster(const std::string & mac, const std::string & ip, const std::string& node_name);

            bool getClusterByMac(const std::string & mac);

            void updateClusterStatus(int status,const std::string& max,  int deleted = 0 );

        private:
            ClusterManageDao() = default;
            ~ClusterManageDao() = default;
        };

    }
}

#endif //MNSX_VALKYRIEHUB_SERVER_CLUSTERMANAGEDAO_H