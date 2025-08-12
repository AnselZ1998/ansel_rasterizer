#include <iostream>
#include <opencv2/opencv.hpp>

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

Eigen::Vector3f normal_fragment_shader(const fragment_shader_payload& payload)
{
    Eigen::Vector3f return_color = (payload.normal.head<3>().normalized() + Eigen::Vector3f(1.0f, 1.0f, 1.0f)) / 2.f;
    Eigen::Vector3f result;
    result << return_color.x() * 255, return_color.y() * 255, return_color.z() * 255;
    return result;
}

static Eigen::Vector3f reflect(const Eigen::Vector3f& vec, const Eigen::Vector3f& axis)
{
    auto costheta = vec.dot(axis);
    return (2 * costheta * axis - vec).normalized();
}

struct light
{
    Eigen::Vector3f position;
    Eigen::Vector3f intensity;
};

Eigen::Vector3f texture_fragment_shader(const fragment_shader_payload& payload)
{
    Eigen::Vector3f return_color = {0, 0, 0};
    if (payload.texture)
    {
        // TODO: Get the texture value at the texture coordinates of the current fragment
        return_color = payload.texture->getColor(payload.tex_coords.x(), payload.tex_coords.y());

    }
    Eigen::Vector3f texture_color;
    texture_color << return_color.x(), return_color.y(), return_color.z();

    Eigen::Vector3f ka = Eigen::Vector3f(0.005, 0.005, 0.005);
    Eigen::Vector3f kd = texture_color / 255.f;
    Eigen::Vector3f ks = Eigen::Vector3f(0.7937, 0.7937, 0.7937);

    auto l1 = light{{20, 20, 20}, {500, 500, 500}};
    auto l2 = light{{-20, 20, 0}, {500, 500, 500}};

    std::vector<light> lights = {l1, l2};
    Eigen::Vector3f amb_light_intensity{10, 10, 10};
    Eigen::Vector3f eye_pos{0, 0, 10};

    float p = 150;

    Eigen::Vector3f color = texture_color;
    Eigen::Vector3f point = payload.view_pos;
    Eigen::Vector3f normal = payload.normal;

    Eigen::Vector3f result_color = {0, 0, 0};
    Eigen::Vector3f ambient = ka.cwiseProduct(amb_light_intensity);

    for (auto& light : lights)
    {
        // TODO: For each light source in the code, calculate what the *ambient*, *diffuse*, and *specular* 
        // components are. Then, accumulate that result on the *result_color* object.
        auto i = light.intensity;
        Eigen::Vector3f l = light.position - point;
        float r_square = (l).dot(l);
        float lambert_cosine = std::max(0.0f, normal.normalized().dot(l.normalized()));
        Eigen::Vector3f h = ((eye_pos - point).normalized() + l.normalized());
        float specular_cosine = std::max(0.0f, normal.normalized().dot(h.normalized()));
        Eigen::Vector3f specular_color = ks.cwiseProduct(i / r_square) * std::pow(specular_cosine, p);

        Eigen::Vector3f diffuse_color = kd.cwiseProduct(i / r_square) * lambert_cosine;
        result_color += (ambient + diffuse_color + specular_color);

    }

    return result_color * 255.f;
}

Eigen::Vector3f phong_fragment_shader(const fragment_shader_payload& payload)
{
    Eigen::Vector3f ka = Eigen::Vector3f(0.005, 0.005, 0.005);
    Eigen::Vector3f kd = payload.color;
    Eigen::Vector3f ks = Eigen::Vector3f(0.7937, 0.7937, 0.7937);

    auto l1 = light{{20, 20, 20}, {500, 500, 500}};
    auto l2 = light{{-20, 20, 0}, {500, 500, 500}};

    std::vector<light> lights = {l1, l2};
    Eigen::Vector3f amb_light_intensity{10, 10, 10};
    Eigen::Vector3f eye_pos{0, 0, 10};

    float p = 150;

    Eigen::Vector3f color = payload.color;
    Eigen::Vector3f point = payload.view_pos;
    Eigen::Vector3f normal = payload.normal;

    Eigen::Vector3f result_color = {0, 0, 0};
    for (auto& light : lights)
    {
        // TODO: For each light source in the code, calculate what the *ambient*, *diffuse*, and *specular* 
        // components are. Then, accumulate that result on the *result_color* object.

        // ambient
        Eigen::Vector3f ambient = ka.cwiseProduct(amb_light_intensity);
        
        auto i = light.intensity;
        Eigen::Vector3f l = light.position - point;
        float r_square = (l).dot(l);
        
        Eigen::Vector3f h = ((eye_pos - point).normalized() + l.normalized());
        float lambert_cosine = std::max(0.0f, normal.normalized().dot(l.normalized()));
        float specular_cosine = std::max(0.0f, normal.normalized().dot(h.normalized()));
        
        //diffuse
        

        Eigen::Vector3f diffuse_color = kd.cwiseProduct(i / r_square) * lambert_cosine;
        Eigen::Vector3f specular_color = ks.cwiseProduct(i / r_square) * std::pow(specular_cosine, p);
        result_color += (ambient + diffuse_color + specular_color);
        
    }

    return result_color * 255.f;
}



Eigen::Vector3f displacement_fragment_shader(const fragment_shader_payload& payload)
{
    
    Eigen::Vector3f ka = Eigen::Vector3f(0.005, 0.005, 0.005);
    Eigen::Vector3f kd = payload.color;
    Eigen::Vector3f ks = Eigen::Vector3f(0.7937, 0.7937, 0.7937);

    auto l1 = light{{20, 20, 20}, {500, 500, 500}};
    auto l2 = light{{-20, 20, 0}, {500, 500, 500}};

    std::vector<light> lights = {l1, l2};
    Eigen::Vector3f amb_light_intensity{10, 10, 10};
    Eigen::Vector3f eye_pos{0, 0, 10};

    float p = 150;

    Eigen::Vector3f color = payload.color; 
    Eigen::Vector3f point = payload.view_pos;
    Eigen::Vector3f normal = payload.normal;

    float kh = 0.2, kn = 0.1;
    
    float u = payload.tex_coords.x();
    float v = payload.tex_coords.y();
    float w = payload.texture->width;
    float h_bump = payload.texture->height;

    auto get_h = [&](float u, float v) {
        return payload.texture->getColor(u, v).norm();
    };

    float h0 = get_h(u, v);
    float h1 = get_h(u + 1.0f / w, v);
    float h2 = get_h(u, v + 1.0f / h_bump);

    float x = normal.x(), y = normal.y(), z = normal.z();
    Eigen::Vector3f t(x*y/sqrt(x*x+z*z), sqrt(x*x+z*z), z*y/sqrt(x*x+z*z));
    t.normalize();
    Eigen::Vector3f b = normal.cross(t);
    b.normalize();

    Eigen::Matrix3f TBN;
    TBN.col(0) = t;
    TBN.col(1) = b;
    TBN.col(2) = normal;

    float dU = kh * kn * (h1 - h0);
    float dV = kh * kn * (h2 - h0);

    Eigen::Vector3f ln(-dU, -dV, 1.0f);
    ln.normalize();
    point = point + kn * normal * h0;
    normal = (TBN * ln).normalized();

    Eigen::Vector3f result_color = {0, 0, 0};
    // displacement point

    for (auto& light : lights)
    {
       
        Eigen::Vector3f ambient = ka.cwiseProduct(amb_light_intensity);
        
        auto i = light.intensity;
        Eigen::Vector3f l = light.position - point;
        float r_square = (l).dot(l);
        
        Eigen::Vector3f h = ((eye_pos - point).normalized() + l.normalized());
        float lambert_cosine = std::max(0.0f, normal.dot(l.normalized()));
        float specular_cosine = std::max(0.0f, normal.dot(h.normalized()));
        
        //diffuse
        
        Eigen::Vector3f diffuse_color = kd.cwiseProduct(i / r_square) * lambert_cosine;
        Eigen::Vector3f specular_color = ks.cwiseProduct(i / r_square) * std::pow(specular_cosine, p);
        result_color += (ambient + diffuse_color + specular_color);
        
    }

    return result_color * 255.f;
}


Eigen::Vector3f bump_fragment_shader(const fragment_shader_payload& payload)
{
    

    Eigen::Vector3f ka = Eigen::Vector3f(0.005, 0.005, 0.005);
    Eigen::Vector3f kd = payload.color;
    Eigen::Vector3f ks = Eigen::Vector3f(0.7937, 0.7937, 0.7937);

    auto l1 = light{{20, 20, 20}, {500, 500, 500}};
    auto l2 = light{{-20, 20, 0}, {500, 500, 500}};

    std::vector<light> lights = {l1, l2};
    Eigen::Vector3f amb_light_intensity{10, 10, 10};
    Eigen::Vector3f eye_pos{0, 0, 10};

    float p = 150;

    Eigen::Vector3f color = payload.color; 
    Eigen::Vector3f point = payload.view_pos;
    Eigen::Vector3f normal = payload.normal;



    // TODO: Implement bump mapping here
    float u = payload.tex_coords.x();
    float v = payload.tex_coords.y();
    float w = payload.texture->width;
    float h_bump = payload.texture->height;

    auto get_h = [&](float u, float v) {
        return payload.texture->getColor(u, v)[0];
    };

    float h0 = get_h(u, v);
    float h1 = get_h(u + 1.0f / w, v);
    float h2 = get_h(u, v + 1.0f / h_bump);

    float kh = 0.2f, kn = 0.1f;

    float x = normal.x(), y = normal.y(), z = normal.z();
    Eigen::Vector3f t(x*y/sqrt(x*x+z*z), sqrt(x*x+z*z), z*y/sqrt(x*x+z*z));
    t.normalize();
    Eigen::Vector3f b = normal.cross(t);
    b.normalize();

    Eigen::Matrix3f TBN;
    TBN.col(0) = t;
    TBN.col(1) = b;
    TBN.col(2) = normal;

    float dU = kh * kn * (h1 - h0);
    float dV = kh * kn * (h2 - h0);

    Eigen::Vector3f ln(-dU, -dV, 1.0f);
    ln.normalize();

    normal = (TBN * ln).normalized();

    Eigen::Vector3f result_color = {0, 0, 0};
    for (auto& light : lights)
    {
       
        Eigen::Vector3f ambient = ka.cwiseProduct(amb_light_intensity);
        
        auto i = light.intensity;
        Eigen::Vector3f l = light.position - point;
        float r_square = (l).dot(l);
        
        Eigen::Vector3f h = ((eye_pos - point).normalized() + l.normalized());
        float lambert_cosine = std::max(0.0f, normal.dot(l.normalized()));
        float specular_cosine = std::max(0.0f, normal.dot(h.normalized()));
        
        //diffuse
        
        Eigen::Vector3f diffuse_color = kd.cwiseProduct(i / r_square) * lambert_cosine;
        Eigen::Vector3f specular_color = ks.cwiseProduct(i / r_square) * std::pow(specular_cosine, p);
        result_color += (ambient + diffuse_color + specular_color);
        
    }

    return result_color * 255.f;
}

bool CheckFileExist(const std::string& filepath)
{
    return std::ifstream(filepath).is_open();
}

static std::function<Eigen::Vector3f(const fragment_shader_payload&)> choose_shader(const std::string& name) {
    if (name == "texture_fragment_shader")      return texture_fragment_shader;
    if (name == "phong")        return phong_fragment_shader;
    if (name == "bump")         return bump_fragment_shader;
    if (name == "displacement") return displacement_fragment_shader;
    return phong_fragment_shader;
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
    r.set_texture(Texture(c.texture_config.base_color));

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

    std::function<Eigen::Vector3f(fragment_shader_payload)> active_shader = choose_shader(c.render_config.shader);

    // set shader
    r.set_vertex_shader(vertex_shader);
    r.set_fragment_shader(active_shader);

    int key = 0;
    int frame_count = 0;
    while(key != 27)
    {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);
        r.set_model(get_model_matrix(c.model_config.angle));
        r.set_view(get_view_matrix(c.camera_config.eye));
        r.set_projection(get_projection_matrix(45.0, 1, 0.1, 50));

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

    }
    return 0;
    
}
