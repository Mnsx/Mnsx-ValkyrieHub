# ==================================================
# 顶级工程配置
# ==================================================
cmake_minimum_required(VERSION 3.10)
project(Mnsx-ValkyrieHub-Server LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ==================================================
# 寻找全局系统依赖
# ==================================================
find_package(Threads REQUIRED)
# find_package(OpenCV REQUIRED) # 如果 main.cpp 也要直接用 OpenCV，打开此行

# ==================================================
# 挂载第三方依赖库 (注意拓扑加载顺序)
# ==================================================
# 1. 最底层的日志库
add_subdirectory(third_party/argus_log)

# 2. 依赖日志库的网络和线程池
add_subdirectory(third_party/achilles_link)
add_subdirectory(third_party/kraken_pool)

# 3. 依赖前三者的 RPC 核心
add_subdirectory(third_party/hermes_rpc)

# 4. 独立的视觉处理模块
add_subdirectory(third_party/skuld_vision)

# ==================================================
# 编译主服务端程序
# ==================================================
add_executable(ValkyrieHub-Server main.cpp
        src/utils/MySQLUtils.cpp
        src/core/HubServer.cpp
        include/core/HubServer.h
        include/core/QtMethodDispatcher.h
        src/core/QtMethodDispatcher.cpp
        include/service/SystemLogService.h
        src/service/SystemLogService.cpp
        include/dao/SystemLogDao.h
        src/dao/SystemLogDao.cpp
        include/service/ClusterManageService.h
        src/service/ClusterManageService.cpp
        include/dao/ClusterManageDao.h
        src/dao/ClusterManageDao.cpp
)

# 链接所有子模块到主程序
# 注意：因为你在子模块中巧妙地使用了 PUBLIC，
# 这里只需要链接 HermesRPC，Achilles 和 Argus 的头文件路径就会自动传递给 MainServer
target_link_libraries(ValkyrieHub-Server
        PRIVATE
        HermesRPC
        SkuldVision
        Threads::Threads
        mysqlclient
)

target_include_directories(ValkyrieHub-Server
        PUBLIC
        include
        third_party/achilles_link/include
        third_party/argus_log/include
        third_party/hermes_rpc/include
        third_party/kraken_pool/include
        third_party/skuld_vision/include
        third_party/nlohmann/include
)

message(STATUS "----------------------------------------")
message(STATUS "Valkyrie Server CMake 组装完毕")
message(STATUS "----------------------------------------")