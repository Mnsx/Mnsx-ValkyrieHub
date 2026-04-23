/** 
 * @file FileUtils.h
 * @author Mnsx_x <xx1527030652@gmail.com>
 * @date 2026/4/23
 * @description 
 */
#ifndef MNSX_VALKYRIEHUB_SERVER_FILEUTILS_H
#define MNSX_VALKYRIEHUB_SERVER_FILEUTILS_H

#include <fstream>
#include <vector>
#include <string>
#include <iostream>

// 🌟 极速读取文件到内存字节数组
inline std::vector<unsigned char> readImageFromDisk(const std::string& relative_path) {
    // 1. 打开文件：必须使用 std::ios::binary (二进制模式)
    // 同时使用 std::ios::ate (打开时立刻定位到文件末尾，方便查大小)
    std::ifstream file(relative_path, std::ios::binary | std::ios::ate);

    // 2. 探针：检查文件是否存在或是否有权限打开
    if (!file.is_open()) {
        std::cerr << "💀 [错误] 无法打开图片文件，请检查路径: " << relative_path << std::endl;
        return {}; // 返回空数组
    }

    // 3. 获取文件精确大小
    std::streamsize size = file.tellg();

    // 4. 将光标移回文件头部，准备读取
    file.seekg(0, std::ios::beg);

    // 5. 分配一块恰好能装下整张图的连续内存
    std::vector<unsigned char> buffer(size);

    // 6. 暴力读取：将硬盘数据直接拍进内存
    if (file.read(reinterpret_cast<char*>(buffer.data()), size)) {
        return buffer; // 大获全胜，返回字节流
    } else {
        std::cerr << "💀 [错误] 图片读取中途中断！" << std::endl;
        return {};
    }
}

#endif //MNSX_VALKYRIEHUB_SERVER_FILEUTILS_H