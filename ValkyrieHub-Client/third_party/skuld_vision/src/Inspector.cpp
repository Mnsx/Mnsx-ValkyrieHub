/**
 * @file Inspector.cpp
 * @author Mnsx_x <xx1527030652@gmail.com>
 * @date 2026/4/15
 */
#include "Inspector.h"
#include <opencv2/imgproc.hpp>

using namespace mnsx::skuld;

cv::Mat Inspector::preprocess(const cv::Mat& src) {
    cv::Mat gray, blurred;

    // 将图片转换为灰度图
    if (src.channels() == 3) {
        cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);
    } else {
        gray = src.clone();
    }

    // 高斯滤波，内核数位5x5，高斯值设置0，由OpenCV测试
    cv::GaussianBlur(gray, blurred, cv::Size(5, 5), 0);

    return blurred;
}

cv::Mat Inspector::segment(const cv::Mat& gray) {
    cv::Mat binary;

    // 使用大津法自动寻找最佳阈值，并进行反二进制阈值化
    cv::threshold(gray, binary, 0, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);

    return binary;
}

cv::Mat Inspector::morphologicalOptimize(const cv::Mat& binary) {
    cv::Mat optimized;

    // 创建一个3x3的矩形
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));

    // 闭运算
    cv::morphologyEx(binary, optimized, cv::MORPH_CLOSE, kernel);

    return optimized;
}

std::vector<cv::Rect> Inspector::extractAnomalies(const cv::Mat& binary, double minArea) {
    std::vector<cv::Rect> anomalyRects;
    std::vector<std::vector<cv::Point>> contours;

    // 提取最外层轮廓
    cv::findContours(binary, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    // 遍历所有找到的轮廓
    for (const auto& contour : contours) {
        // 计算轮廓的实际像素面积
        double area = cv::contourArea(contour);

        // 过滤掉面积小于阈值的图形
        if (area > minArea) {
            // 计算并保存该轮廓的最小正外接矩形
            cv::Rect boundingBox = cv::boundingRect(contour);
            anomalyRects.push_back(boundingBox);
        }
    }

    return anomalyRects;
}