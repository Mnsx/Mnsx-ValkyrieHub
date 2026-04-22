/** 
 * @file NodeInfoUtils.h
 * @author Mnsx_x <xx1527030652@gmail.com>
 * @date 2026/4/22
 * @description 
 */
#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <ifaddrs.h>
#include <arpa/inet.h>

// 定义一个结构体来打包返回的数据
struct NodeNetworkInfo {
    std::string ip;
    std::string mac;
};

class NetworkDetector {
public:
    static NodeNetworkInfo getPrimaryNetworkInfo() {
        NodeNetworkInfo info = {"0.0.0.0", "00:00:00:00:00:00"};
        struct ifaddrs *ifaddr = nullptr;
        struct ifaddrs *ifa = nullptr;

        // 获取系统中所有的网络接口列表
        if (getifaddrs(&ifaddr) == -1) {
            return info;
        }

        // 遍历网卡链表
        for (ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next) {
            if (ifa->ifa_addr == nullptr) continue;

            // 只处理IPv4地址
            if (ifa->ifa_addr->sa_family == AF_INET) {
                std::string ifName = ifa->ifa_name;

                //跳过本地环回网卡
                //跳过docker虚拟网卡
                if (ifName == "lo" || ifName.find("docker") != std::string::npos) {
                    continue;
                }

                // 提取 IP 地址
                struct sockaddr_in *pAddr = (struct sockaddr_in *)ifa->ifa_addr;
                char ipStr[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &pAddr->sin_addr, ipStr, INET_ADDRSTRLEN);
                info.ip = ipStr;

                //提取MAC地址
                int fd = socket(AF_INET, SOCK_DGRAM, 0);
                if (fd != -1) {
                    struct ifreq ifr;
                    memset(&ifr, 0, sizeof(ifr));
                    // 将网卡名字填进去，告诉内核我们要查哪个网卡
                    strncpy(ifr.ifr_name, ifName.c_str(), IFNAMSIZ - 1);

                    if (ioctl(fd, SIOCGIFHWADDR, &ifr) == 0) {
                        unsigned char *mac = (unsigned char *)ifr.ifr_hwaddr.sa_data;
                        char macStr[18];
                        snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
                                 mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
                        info.mac = macStr;
                    }
                    close(fd);
                }

                break;
            }
        }

        freeifaddrs(ifaddr);
        return info;
    }
};