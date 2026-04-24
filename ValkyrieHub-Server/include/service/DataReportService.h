/** 
 * @file DataReportService.h
 * @author Mnsx_x <xx1527030652@gmail.com>
 * @date 2026/4/24
 * @description 
 */
#ifndef MNSX_VALKYRIEHUB_SERVER_DATAREPORTSERVICE_H
#define MNSX_VALKYRIEHUB_SERVER_DATAREPORTSERVICE_H

#include "nlohmann/json.hpp"

using Json = nlohmann::json;

namespace mnsx {
    namespace valkyrie {

        class DataReportService {
        public:
            static DataReportService& getInstance() {
                static DataReportService instance;
                return instance;;
            }
            Json getRealtimeYield();

            Json falsePostitiveRateWeekly();

        private:
            DataReportService() = default;
            ~DataReportService() = default;
        };

    }
}

#endif //MNSX_VALKYRIEHUB_SERVER_DATAREPORTSERVICE_H