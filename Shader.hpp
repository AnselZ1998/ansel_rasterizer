//
// Created by LEI XU on 4/27/19.
//

#ifndef RASTERIZER_SHADER_H
#define RASTERIZER_SHADER_H
#include <eigen3/Eigen/Eigen>
#include <functional>
#include "Texture.hpp"


struct fragment_shader_payload
{
    fragment_shader_payload()
    {
        basecolor_texture = nullptr;
        displacement_texture = nullptr;
    }

    fragment_shader_payload(const Eigen::Vector3f& col, const Eigen::Vector3f& nor,const Eigen::Vector2f& tc, Texture* basecolor_tex, Texture* displacement_tex) :
         color(col), normal(nor), tex_coords(tc), basecolor_texture(basecolor_tex), displacement_texture(displacement_tex) {}

    Eigen::Vector3f view_pos;
    Eigen::Vector3f color;
    Eigen::Vector3f normal;
    Eigen::Vector2f tex_coords;
    Eigen::Vector3f amb_light_intensity;
    Texture* basecolor_texture;
    Texture* displacement_texture;

    // ShaderOption
    bool useTexture = true;
    bool useDisplacement = true;
    float kh = 0.6;
    float kn = 0.5;
    float shininess;
    Eigen::Vector3f ka = {0.005f, 0.005f, 0.005f};
    Eigen::Vector3f ks = {0.7937f, 0.7937f, 0.7937f};
};

struct vertex_shader_payload
{
    Eigen::Vector3f position;
};

struct light
{
    Eigen::Vector3f position;
    Eigen::Vector3f intensity;
};

Eigen::Vector3f ansel_standard_shader(const fragment_shader_payload& payload);

#endif //RASTERIZER_SHADER_H
