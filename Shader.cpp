#include "Shader.hpp"

Eigen::Vector3f ansel_standard_shader(const fragment_shader_payload& payload)
{
    // -------- 基础取样：basecolor（0~255 → 0~1）--------
    Eigen::Vector3f kd = Eigen::Vector3f(0.5f, 0.5f, 0.5f); // 没有贴图的保底漫反射
    if (payload.basecolor_texture)
    {
        Eigen::Vector3f tex = payload.basecolor_texture->getColor(payload.tex_coords.x(), payload.tex_coords.y());
        kd = tex / 255.0f;
    }

    // 材质参数（已由上游写入 payload）
    Eigen::Vector3f ka = payload.ka;     // 环境
    Eigen::Vector3f ks = payload.ks;     // 高光
    float kh = payload.kh;               // 凹凸强度
    float kn = payload.kn;               // 位移强度

    // 光源与环境光
    auto l1 = light{{20, 20, 20}, {500, 500, 500}};
    auto l2 = light{{-20, 20,  0}, {500, 500, 500}};
    std::vector<light> lights = {l1, l2};
    Eigen::Vector3f amb = ka.cwiseProduct(payload.amb_light_intensity); // 只加一次

    // 视点、Phong 指数
    Eigen::Vector3f eye_pos{0, 0, 10};
    float p = 70.0f;

    // 准备几何量（view space）
    Eigen::Vector3f point  = payload.view_pos;
    Eigen::Vector3f normal = payload.normal.normalized();

    // -------- 位移 / 凹凸处理（判空更安全）--------
    if (payload.displacement_texture)
    {
        // 纹理步长（避免除零）
        const float w = std::max(1.0f, static_cast<float>(payload.displacement_texture->width));
        const float h = std::max(1.0f, static_cast<float>(payload.displacement_texture->height));
        const float u = payload.tex_coords.x();
        const float v = payload.tex_coords.y();

        auto height01 = [&](float uu, float vv) -> float {
            // 用灰度的均值做高度，映射到 [0,1]
            Eigen::Vector3f c = payload.displacement_texture->getColor(uu, vv);
            return (c.x() + c.y() + c.z()) / (3.0f * 255.0f);
        };

        float h0 = height01(u, v);
        float h1 = height01(u + 1.0f / w, v);
        float h2 = height01(u, v + 1.0f / h);

        // 构建稳健 TBN：避免与法线共线导致的除零
        Eigen::Vector3f n = normal;
        Eigen::Vector3f t;
        if (std::abs(n.z()) < 0.999f) t = Eigen::Vector3f(0,0,1).cross(n).normalized();
        else                           t = Eigen::Vector3f(0,1,0).cross(n).normalized();
        Eigen::Vector3f b = n.cross(t);

        Eigen::Matrix3f TBN;
        TBN.col(0) = t; TBN.col(1) = b; TBN.col(2) = n;

        float dU = kh * kn * (h1 - h0);
        float dV = kh * kn * (h2 - h0);

        // 局部法线（切线空间凹凸）
        Eigen::Vector3f ln(-dU, -dV, 1.0f);
        ln.normalize();

        // 位移顶点：沿原法线方向挪动（注意 h0 已是 [0,1]）
        point  = point + kn * n * h0;
        normal = (TBN * ln).normalized();
    }

    // -------- 光照累计（Blinn-Phong）--------
    Eigen::Vector3f result = amb; // 环境光只叠加一次
    for (const auto& L : lights)
    {
        Eigen::Vector3f l = (L.position - point);
        float r2 = std::max(1e-6f, l.squaredNorm());
        l.normalize();

        // 半程向量
        Eigen::Vector3f v = (eye_pos - point).normalized();
        Eigen::Vector3f hvec = (v + l).normalized();

        float NdotL = std::max(0.0f, normal.dot(l));
        float NdotH = std::max(0.0f, normal.dot(hvec));

        Eigen::Vector3f diffuse  = kd.cwiseProduct(L.intensity / r2) * NdotL;
        Eigen::Vector3f specular = ks.cwiseProduct(L.intensity / r2) * std::pow(NdotH, p);

        result += diffuse + specular;
    }

    // 返回到 0~255
    return result * 255.0f;
}
