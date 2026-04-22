/** 
 * @file HubClient.h
 * @author Mnsx_x <xx1527030652@gmail.com>
 * @date 2026/4/22
 * @description 
 */
#ifndef MNSX_VALKYRIEHUB_HUBCLIENT_H
#define MNSX_VALKYRIEHUB_HUBCLIENT_H

#include <string>
#include <cstdint>

#include "process/CameraOperator.h"

namespace mnsx {
    namespace valkyrie {

        static const std::string SERVER_IP = "0.0.0.0";
        constexpr static const int SERVER_PORT = 8080;

        class HubClient {
        public:
            explicit HubClient();
            ~HubClient() = default;
        private:
            CameraOperator operator_;
        };

    }
}

#endif //MNSX_VALKYRIEHUB_HUBCLIENT_H