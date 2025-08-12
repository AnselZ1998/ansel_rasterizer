//
// Created by LEI XU on 4/27/19.
//

#ifndef RASTERIZER_TEXTURE_H
#define RASTERIZER_TEXTURE_H
#include "global.hpp"
#include <eigen3/Eigen/Eigen>
#include <opencv2/opencv.hpp>
class Texture{
private:
    cv::Mat image_data;

public:
    Texture(const std::string& name)
    {
        std::cout << "Texture Path: " << name <<std::endl;
        image_data = cv::imread(name);
        cv::cvtColor(image_data, image_data, cv::COLOR_RGB2BGR);
        width = image_data.cols;
        height = image_data.rows;
    }

    int width, height;

    Eigen::Vector3f getColor(float u, float v)
    {
        auto u_img = u * (width - 1);
        auto v_img = (1 - v) * (height - 1);
        auto color = image_data.at<cv::Vec3b>(v_img, u_img);
        return Eigen::Vector3f(color[0], color[1], color[2]);
    }

    Eigen::Vector3f getColorBilinear(float u, float v)
{
    using std::cout; using std::endl;

    cout << "--------------------- start ----------------\n";
    cout << std::fixed << std::setprecision(4);
    cout << "input u=" << u << ", v=" << v << endl;

    // 0) clamp 到 [0,1]，注意用 float 常量
    u = std::max(0.0f, std::min(u, 1.0f));
    v = std::max(0.0f, std::min(v, 1.0f));
    cout << "clamped u=" << u << ", v=" << v << endl;

    // 1) UV -> 连续图像坐标（左上原点，v 翻转一次）
    float x = u * (width  - 1);
    float y = (1.0f - v) * (height - 1);
    cout << "continuous x=" << x << ", y=" << y << endl;

    // 2) 左上整数索引
    int x0 = static_cast<int>(std::floor(x));
    int y0 = static_cast<int>(std::floor(y));

    // 3) 右/下邻（边界 clamp）
    int x1 = std::min(x0 + 1, width  - 1);
    int y1 = std::min(y0 + 1, height - 1);
    cout << "indices: (x0,y0)=(" << x0 << "," << y0 << "), (x1,y1)=(" << x1 << "," << y1 << ")\n";

    // 4) 小数部分 = 在该小格内的相对位置 [0,1)
    float a = x - x0; // 水平方向比例（向右）
    float b = y - y0; // 垂直方向比例（向下）
    cout << "fractions: a=" << a << " (horiz), b=" << b << " (vert)\n";

    // 5) 取四个角（OpenCV: at(row,col) = at(y,x)）
    cv::Vec3b c_no_bilinear = image_data.at<cv::Vec3b>(y, x); // no_bilinear
    cv::Vec3b c00 = image_data.at<cv::Vec3b>(y0, x0); // TL
    cv::Vec3b c10 = image_data.at<cv::Vec3b>(y0, x1); // TR
    cv::Vec3b c01 = image_data.at<cv::Vec3b>(y1, x0); // BL
    cv::Vec3b c11 = image_data.at<cv::Vec3b>(y1, x1); // BR

    Eigen::Vector3f C_no_bilinear(c_no_bilinear[0], c_no_bilinear[1], c_no_bilinear[2]);
    Eigen::Vector3f C00(c00[0], c00[1], c00[2]);
    Eigen::Vector3f C10(c10[0], c10[1], c10[2]);
    Eigen::Vector3f C01(c01[0], c01[1], c01[2]);
    Eigen::Vector3f C11(c11[0], c11[1], c11[2]);

    cout << "C00(BGR)=" << C00.transpose() << ", C10=" << C10.transpose()
         << ", C01=" << C01.transpose() << ", C11=" << C11.transpose() << endl;

    // 6) 先横向两次插值（上行、下行）
    Eigen::Vector3f C0 = (1 - a) * C00 + a * C10; // 上边：左上->右上
    Eigen::Vector3f C1 = (1 - a) * C01 + a * C11; // 下边：左下->右下
    cout << "C0(top row) =" << C0.transpose() << "\n";
    cout << "C1(bottom)="  << C1.transpose() << "\n";

    // 7) 再竖向插值（上行->下行）
    Eigen::Vector3f Color = (1 - b) * C0 + b * C1;
    cout << "Color(bilinear)=" << Color.transpose() << endl;
    cout << "Color(no_bilinear)=" << C_no_bilinear.transpose() << endl;
    cout << "---------------------- end ----------------\n";

    return Color; // 直接返回插值颜色（BGR顺序）
}


};
#endif //RASTERIZER_TEXTURE_H
