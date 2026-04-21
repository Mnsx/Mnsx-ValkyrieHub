/** 
 * @file SystemLogService.h
 * @author Mnsx_x <xx1527030652@gmail.com>
 * @date 2026/4/21
 * @description 
 */
#ifndef MNSX_VALKYRIEHUB_SERVER_SYSTEMLOGSERVICE_H
#define MNSX_VALKYRIEHUB_SERVER_SYSTEMLOGSERVICE_H
#include <string>
#include "nlohmann/json.hpp"

using Json = nlohmann::json;

namespace mnsx {
    namespace valkyrie {

        class SystemLogService {
        public:
            static SystemLogService& getInstance() {
                static SystemLogService instance;
                return instance;
            }

            Json getSystemLog();
        private:
            SystemLogService() = default;
            ~SystemLogService() = default;
        };

    }
}

#endif //MNSX_VALKYRIEHUB_SERVER_SYSTEMLOGSERVICE_H