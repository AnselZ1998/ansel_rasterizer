#include <iostream>
#include <opencv2/opencv.hpp>
#include <functional>
#include <fstream>

#include "global.hpp"
#include "rasterizer.hpp"
#include "Triangle.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "OBJ_Loader.h"
#include "nlohmann/json.hpp"
#include "Config.hpp"

Eigen::Matrix4f get_view_matrix(Eigen::Vector3f eye_pos)
{
    Eigen::Matrix4f view = Eigen::Matrix4f::Identity();

    Eigen::Matrix4f translate;
    translate << 1,0,0,-eye_pos[0],
                 0,1,0,-eye_pos[1],
                 0,0,1,-eye_pos[2],
                 0,0,0,1;

    view = translate*view;

    return view;
}

Eigen::Matrix4f get_model_matrix(float angle)
{
    Eigen::Matrix4f rotation;
    angle = angle * MY_PI / 180.f;
    rotation << cos(angle), 0, sin(angle), 0,
                0, 1, 0, 0,
                -sin(angle), 0, cos(angle), 0,
                0, 0, 0, 1;

    Eigen::Matrix4f scale;
    scale << 2.5, 0, 0, 0,
              0, 2.5, 0, 0,
              0, 0, 2.5, 0,
              0, 0, 0, 1;

    Eigen::Matrix4f translate;
    translate << 1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1;

    return translate * rotation * scale;
}

Eigen::Matrix4f get_projection_matrix(float eye_fov, float aspect_ratio, float zNear, float zFar)
{
    float theta = (eye_fov / 2.0f) * MY_PI / 180.0f;
    float cot_theta = 1.0f / tan(theta);

    Eigen::Matrix4f projection = Eigen::Matrix4f::Zero();

    projection(0, 0) = cot_theta / aspect_ratio;
    projection(1, 1) = cot_theta;
    projection(2, 2) = (zFar + zNear) / (zNear - zFar);
    projection(2, 3) = (2.0f * zNear * zFar) / (zNear - zFar);
    projection(3, 2) = -1.0f;

    return projection;
}

Eigen::Vector3f vertex_shader(const vertex_shader_payload& payload)
{
    return payload.position;
}

bool CheckFileExist(const std::string& filepath)
{
    return std::ifstream(filepath).is_open();
}

int main(int argc, const char** argv)
{   
    // init Triangle
    std::vector<Triangle*> TriangleList;

    // init default data
    Config c;

    // init rasterizer
    rst::rasterizer r(c.render_config.height, c.render_config.width);

    // init Loader
    objl::Loader Loader;

    // set texture
    if (!CheckFileExist(c.texture_config.base_color))
    {
        std::cout << "Config File Can Not Open: " << c.texture_config.base_color << std::endl;
        return -1;
    }
    r.set_basecolor_texture(Texture(c.texture_config.base_color));

    // set .obj File
    if (!CheckFileExist(c.model_config.obj))
    {
        std::cout << "Config File Can Not Open: " << c.model_config.obj << std::endl;
        return -1;
    }

    // load model file
    bool loadout = Loader.LoadFile(c.model_config.obj);
    for(auto mesh:Loader.LoadedMeshes)
    {
        for(int i=0;i<mesh.Vertices.size();i+=3)
        {
            Triangle* t = new Triangle();
            for(int j=0;j<3;j++)
            {
                t->setVertex(j,Vector4f(mesh.Vertices[i+j].Position.X,mesh.Vertices[i+j].Position.Y,mesh.Vertices[i+j].Position.Z,1.0));
                t->setNormal(j,Vector3f(mesh.Vertices[i+j].Normal.X,mesh.Vertices[i+j].Normal.Y,mesh.Vertices[i+j].Normal.Z));
                t->setTexCoord(j,Vector2f(mesh.Vertices[i+j].TextureCoordinate.X, mesh.Vertices[i+j].TextureCoordinate.Y));
            }
            TriangleList.push_back(t);
        }
    }

    // std::function<Eigen::Vector3f(const fragment_shader_payload&)> active_shader = choose_shader(c.render_config.shader);

    // set shader
    r.set_vertex_shader(vertex_shader);
    r.set_fragment_shader(ansel_standard_shader());

    int key = 0;
    int frame_count = 0;
    while(key != 27)
    {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);
        r.set_model(get_model_matrix(c.model_config.angle));
        r.set_view(get_view_matrix(c.camera_config.eye));
        r.set_projection(get_projection_matrix(45.0, 1, 0.1, 50));
        r.set_amb_light_intensity(c.render_config.amb_light_intensity);

        //r.draw(pos_id, ind_id, col_id, rst::Primitive::Triangle);
        r.draw(TriangleList);
        cv::Mat image(c.render_config.height, c.render_config.width, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);
        cv::cvtColor(image, image, cv::COLOR_RGB2BGR);

        cv::imshow("image", image);
        key = cv::waitKey(10);
        if (key == 'a' )
        {
            c.model_config.angle -= 20;
            std::cout << "angle-20" <<std::endl;
            
        }
        else if (key == 'd')
        {
            c.model_config.angle += 20;
            std::cout << "angle +20" <<std::endl;
            
        }
        else if (key == 'w')
        {
            c.render_config.amb_light_intensity = (c.render_config.amb_light_intensity + Eigen::Vector3f::Constant(2.f)).cwiseMax(0.0f);

            std::cout << "amb_light += 2 -> "
                    << c.render_config.amb_light_intensity.transpose() << '\n';
        }


        else if (key == 's')
        {
            c.render_config.amb_light_intensity = (c.render_config.amb_light_intensity - Eigen::Vector3f::Constant(2.f)).cwiseMax(0.0f);

            std::cout << "amb_light -= 2 -> "
                    << c.render_config.amb_light_intensity.transpose() << '\n';
        }

    }
    return 0;
    
}
