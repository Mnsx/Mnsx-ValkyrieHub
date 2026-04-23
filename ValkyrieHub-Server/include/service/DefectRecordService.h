/** 
 * @file DefectRecord.h
 * @author Mnsx_x <xx1527030652@gmail.com>
 * @date 2026/4/23
 * @description 
 */
#ifndef MNSX_VALKYRIEHUB_SERVER_DEFECTRECORD_H
#define MNSX_VALKYRIEHUB_SERVER_DEFECTRECORD_H

#include <string>
#include <vector>

#include "dao/ClusterManageDao.h"

namespace mnsx {
    namespace valkyrie {

        class DefectRecordService {
        public:
            static DefectRecordService& getInstance() {
                static DefectRecordService instance;
                return instance;
            }

            bool addNewDefectRecord(std::vector<std::string> params);

            Json getAllDetectRecord();

        private:
            DefectRecordService() = default;
            ~DefectRecordService() = default;
        };

    }
}

#endif //MNSX_VALKYRIEHUB_SERVER_DEFECTRECORD_H