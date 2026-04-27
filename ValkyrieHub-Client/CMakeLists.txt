# ==================================================
# 顶级工程配置
# ==================================================
cmake_minimum_required(VERSION 3.10)
project(Mnsx-ValkyrieHub LANGUAGES CXX)

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
set(ORT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/onnx")

# ==================================================
# 编译主服务端程序
# ==================================================
add_executable(ValkyrieClient main.cpp
        include/core/HubClient.h
        src/core/HubClient.cpp
        include/utils/NetworkDetector.h
        include/core/process/CameraOperator.h
        src/core/process/CameraOperator.cpp
)

target_link_directories(ValkyrieClient PRIVATE third_party/onnx/lib)

# 链接所有子模块到主程序
# 注意：因为你在子模块中巧妙地使用了 PUBLIC，
# 这里只需要链接 HermesRPC，Achilles 和 Argus 的头文件路径就会自动传递给 MainServer
target_link_libraries(ValkyrieClient
        PRIVATE
        HermesRPC
        SkuldVision
        Threads::Threads
        uuid
        onnxruntime
)

target_include_directories(ValkyrieClient
        PUBLIC
        include
        third_party/onnx/include
        third_party/achilles_link/include
        third_party/argus_log/include
        third_party/hermes_rpc/include
        third_party/kraken_pool/include
        third_party/skuld_vision/include
)