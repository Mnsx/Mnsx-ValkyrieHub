/** 
 * @file DefectRecordDao.h
 * @author Mnsx_x <xx1527030652@gmail.com>
 * @date 2026/4/23
 * @description 
 */
#ifndef MNSX_VALKYRIEHUB_SERVER_DEFECTRECORDDAO_H
#define MNSX_VALKYRIEHUB_SERVER_DEFECTRECORDDAO_H

#include <string>
#include <vector>

#include "ClusterManageDao.h"

namespace mnsx {
    namespace valkyrie {

        class DefectRecordDao {
        public:
            static DefectRecordDao& getInstance() {
                static DefectRecordDao instance;
                return instance;
            }

            bool insertDefectRecord(std::vector<std::string> params);

            Json selectAll();

            void updateRecordStatus(const std::string & flag, const std::string & string);

        private:
            DefectRecordDao() = default;
            ~DefectRecordDao() = default;
        };
    }
}

#endif //MNSX_VALKYRIEHUB_SERVER_DEFECTRECORDDAO_H