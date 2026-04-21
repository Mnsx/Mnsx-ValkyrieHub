/** 
 * @file SystemLogDao.h
 * @author Mnsx_x <xx1527030652@gmail.com>
 * @date 2026/4/21
 * @description 
 */
#ifndef MNSX_VALKYRIEHUB_SERVER_SYSTEMLOGDAO_H
#define MNSX_VALKYRIEHUB_SERVER_SYSTEMLOGDAO_H

#include <string>

#include "core/QtMethodDispatcher.h"
#include "utils/MySQLUtils.h"
#include "nlohmann/json.hpp"

using Json = nlohmann::json;

namespace mnsx {
    namespace valkyrie {

        class SystemLogDao {
        public:
            static SystemLogDao& getInstance() {

                static SystemLogDao instance;
                return instance;
            }

            Json selectAll();

        private:
            SystemLogDao() = default;
            ~SystemLogDao() = default;
        };

    }
}

#endif //MNSX_VALKYRIEHUB_SERVER_SYSTEMLOGDAO_H