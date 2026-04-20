/** 
 * @file DbLogHandler.h
 * @author Mnsx_x <xx1527030652@gmail.com>
 * @date 2026/4/20
 * @description 
 */
#ifndef MNSX_VALKYRIEHUB_SERVER_DBLOGHANDLER_H
#define MNSX_VALKYRIEHUB_SERVER_DBLOGHANDLER_H
#include "ILogHandler.h"
#include "../../../src/utils/MySQLUtils.h"

namespace mnsx {
    namespace argus {

        class DbLogHandler : public ILogHandler {
        public:
            void handle(const LogEvent &event) override;
        };

    }
}

#endif //MNSX_VALKYRIEHUB_SERVER_DBLOGHANDLER_H