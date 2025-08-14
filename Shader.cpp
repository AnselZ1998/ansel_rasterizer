#include "Shader.hpp"

std::function<Eigen::Vector3f(const fragment_shader_payload&)> ansel_standard_shader(
    const ShaderOptions& opt,
    const std::vector<light>& lights,
    const Eigen::Vector3f& eye_pos
)
{

    Eigen::Vector3f return_color = (payload.normal.head<3>().normalized() + Eigen::Vector3f(1.0f, 1.0f, 1.0f)) / 2.f;
    Eigen::Vector3f result;
    result << return_color.x() * 255, return_color.y() * 255, return_color.z() * 255;
    return result;

}

// Eigen::Vector3f normal_fragment_shader(const fragment_shader_payload& payload)
// {
//     Eigen::Vector3f return_color = (payload.normal.head<3>().normalized() + Eigen::Vector3f(1.0f, 1.0f, 1.0f)) / 2.f;
//     Eigen::Vector3f result;
//     result << return_color.x() * 255, return_color.y() * 255, return_color.z() * 255;
//     return result;
// }

// static Eigen::Vector3f reflect(const Eigen::Vector3f& vec, const Eigen::Vector3f& axis)
// {
//     auto costheta = vec.dot(axis);
//     return (2 * costheta * axis - vec).normalized();
// }

// struct light
// {
//     Eigen::Vector3f position;
//     Eigen::Vector3f intensity;
// };

// Eigen::Vector3f texture_fragment_shader(const fragment_shader_payload& payload)
// {
//     Eigen::Vector3f return_color = {0, 0, 0};
//     if (payload.texture)
//     {
//         // TODO: Get the texture value at the texture coordinates of the current fragment
//         return_color = payload.texture->getColor(payload.tex_coords.x(), payload.tex_coords.y());

//     }
//     Eigen::Vector3f texture_color;
//     texture_color << return_color.x(), return_color.y(), return_color.z();

//     Eigen::Vector3f ka = Eigen::Vector3f(0.005, 0.005, 0.005);
//     Eigen::Vector3f kd = texture_color / 255.f;
//     Eigen::Vector3f ks = Eigen::Vector3f(0.7937, 0.7937, 0.7937);

//     auto l1 = light{{20, 20, 20}, {500, 500, 500}};
//     auto l2 = light{{-20, 20, 0}, {500, 500, 500}};

//     std::vector<light> lights = {l1, l2};
//     Eigen::Vector3f amb_light_intensity = payload.amb_light_intensity;
//     Eigen::Vector3f eye_pos{0, 0, 10};

//     float p = 150;

//     Eigen::Vector3f color = texture_color;
//     Eigen::Vector3f point = payload.view_pos;
//     Eigen::Vector3f normal = payload.normal;

//     Eigen::Vector3f result_color = {0, 0, 0};
//     Eigen::Vector3f ambient = ka.cwiseProduct(amb_light_intensity);

//     for (auto& light : lights)
//     {
//         // TODO: For each light source in the code, calculate what the *ambient*, *diffuse*, and *specular* 
//         // components are. Then, accumulate that result on the *result_color* object.
//         auto i = light.intensity;
//         Eigen::Vector3f l = light.position - point;
//         float r_square = (l).dot(l);
//         float lambert_cosine = std::max(0.0f, normal.normalized().dot(l.normalized()));
//         Eigen::Vector3f h = ((eye_pos - point).normalized() + l.normalized());
//         float specular_cosine = std::max(0.0f, normal.normalized().dot(h.normalized()));
//         Eigen::Vector3f specular_color = ks.cwiseProduct(i / r_square) * std::pow(specular_cosine, p);

//         Eigen::Vector3f diffuse_color = kd.cwiseProduct(i / r_square) * lambert_cosine;
//         result_color += (ambient + diffuse_color + specular_color);

//     }

//     return result_color * 255.f;
// }

// Eigen::Vector3f phong_fragment_shader(const fragment_shader_payload& payload)
// {
//     Eigen::Vector3f ka = Eigen::Vector3f(0.005, 0.005, 0.005);
//     Eigen::Vector3f kd = payload.color;
//     Eigen::Vector3f ks = Eigen::Vector3f(0.7937, 0.7937, 0.7937);

//     auto l1 = light{{20, 20, 20}, {500, 500, 500}};
//     auto l2 = light{{-20, 20, 0}, {500, 500, 500}};

//     std::vector<light> lights = {l1, l2};
//     Eigen::Vector3f amb_light_intensity = payload.amb_light_intensity;
//     //Eigen::Vector3f amb_light_intensity{10, 10, 10};
//     Eigen::Vector3f eye_pos{0, 0, 10};

//     float p = 30;

//     Eigen::Vector3f color = payload.color;
//     Eigen::Vector3f point = payload.view_pos;
//     Eigen::Vector3f normal = payload.normal;

//     Eigen::Vector3f result_color = {0, 0, 0};
//     for (auto& light : lights)
//     {
//         // TODO: For each light source in the code, calculate what the *ambient*, *diffuse*, and *specular* 
//         // components are. Then, accumulate that result on the *result_color* object.

//         // ambient
//         Eigen::Vector3f ambient = ka.cwiseProduct(amb_light_intensity);
        
//         auto i = light.intensity;
//         Eigen::Vector3f l = light.position - point;
//         float r_square = (l).dot(l);
        
//         Eigen::Vector3f h = ((eye_pos - point).normalized() + l.normalized());
//         float lambert_cosine = std::max(0.0f, normal.normalized().dot(l.normalized()));
//         float specular_cosine = std::max(0.0f, normal.normalized().dot(h.normalized()));
        
//         //diffuse
        
//         Eigen::Vector3f diffuse_color = kd.cwiseProduct(i / r_square) * lambert_cosine;
//         Eigen::Vector3f specular_color = ks.cwiseProduct(i / r_square) * std::pow(specular_cosine, p);
//         result_color += (ambient + diffuse_color + specular_color);
        
//     }

//     return result_color * 255.f;
// }



// Eigen::Vector3f displacement_fragment_shader(const fragment_shader_payload& payload)
// {
    
//     Eigen::Vector3f ka = Eigen::Vector3f(0.005, 0.005, 0.005);
//     Eigen::Vector3f kd = payload.color;
//     Eigen::Vector3f ks = Eigen::Vector3f(0.7937, 0.7937, 0.7937);

//     auto l1 = light{{20, 20, 20}, {500, 500, 500}};
//     auto l2 = light{{-20, 20, 0}, {500, 500, 500}};

//     std::vector<light> lights = {l1, l2};
//     Eigen::Vector3f amb_light_intensity{10, 10, 10};
//     Eigen::Vector3f eye_pos{0, 0, 10};

//     float p = 150;

//     Eigen::Vector3f color = payload.color; 
//     Eigen::Vector3f point = payload.view_pos;
//     Eigen::Vector3f normal = payload.normal;

//     float kh = 0.2, kn = 0.1;
    
//     float u = payload.tex_coords.x();
//     float v = payload.tex_coords.y();
//     float w = payload.texture->width;
//     float h_bump = payload.texture->height;

//     auto get_h = [&](float u, float v) {
//         return payload.texture->getColor(u, v).norm();
//     };

//     float h0 = get_h(u, v);
//     float h1 = get_h(u + 1.0f / w, v);
//     float h2 = get_h(u, v + 1.0f / h_bump);

//     float x = normal.x(), y = normal.y(), z = normal.z();
//     Eigen::Vector3f t(x*y/sqrt(x*x+z*z), sqrt(x*x+z*z), z*y/sqrt(x*x+z*z));
//     t.normalize();
//     Eigen::Vector3f b = normal.cross(t);
//     b.normalize();

//     Eigen::Matrix3f TBN;
//     TBN.col(0) = t;
//     TBN.col(1) = b;
//     TBN.col(2) = normal;

//     float dU = kh * kn * (h1 - h0);
//     float dV = kh * kn * (h2 - h0);

//     Eigen::Vector3f ln(-dU, -dV, 1.0f);
//     ln.normalize();
//     point = point + kn * normal * h0;
//     normal = (TBN * ln).normalized();

//     Eigen::Vector3f result_color = {0, 0, 0};
//     // displacement point

//     for (auto& light : lights)
//     {
       
//         Eigen::Vector3f ambient = ka.cwiseProduct(amb_light_intensity);
        
//         auto i = light.intensity;
//         Eigen::Vector3f l = light.position - point;
//         float r_square = (l).dot(l);
        
//         Eigen::Vector3f h = ((eye_pos - point).normalized() + l.normalized());
//         float lambert_cosine = std::max(0.0f, normal.dot(l.normalized()));
//         float specular_cosine = std::max(0.0f, normal.dot(h.normalized()));
        
//         //diffuse
        
//         Eigen::Vector3f diffuse_color = kd.cwiseProduct(i / r_square) * lambert_cosine;
//         Eigen::Vector3f specular_color = ks.cwiseProduct(i / r_square) * std::pow(specular_cosine, p);
//         result_color += (ambient + diffuse_color + specular_color);
        
//     }

//     return result_color * 255.f;
// }


// Eigen::Vector3f bump_fragment_shader(const fragment_shader_payload& payload)
// {
    

//     Eigen::Vector3f ka = Eigen::Vector3f(0.005, 0.005, 0.005);
//     Eigen::Vector3f kd = payload.color;
//     Eigen::Vector3f ks = Eigen::Vector3f(0.7937, 0.7937, 0.7937);

//     auto l1 = light{{20, 20, 20}, {500, 500, 500}};
//     auto l2 = light{{-20, 20, 0}, {500, 500, 500}};

//     std::vector<light> lights = {l1, l2};
//     Eigen::Vector3f amb_light_intensity{10, 10, 10};
//     Eigen::Vector3f eye_pos{0, 0, 10};

//     float p = 150;

//     Eigen::Vector3f color = payload.color; 
//     Eigen::Vector3f point = payload.view_pos;
//     Eigen::Vector3f normal = payload.normal;



//     // TODO: Implement bump mapping here
//     float u = payload.tex_coords.x();
//     float v = payload.tex_coords.y();
//     float w = payload.texture->width;
//     float h_bump = payload.texture->height;

//     auto get_h = [&](float u, float v) {
//         return payload.texture->getColor(u, v)[0];
//     };

//     float h0 = get_h(u, v);
//     float h1 = get_h(u + 1.0f / w, v);
//     float h2 = get_h(u, v + 1.0f / h_bump);

//     float kh = 0.2f, kn = 0.1f;

//     float x = normal.x(), y = normal.y(), z = normal.z();
//     Eigen::Vector3f t(x*y/sqrt(x*x+z*z), sqrt(x*x+z*z), z*y/sqrt(x*x+z*z));
//     t.normalize();
//     Eigen::Vector3f b = normal.cross(t);
//     b.normalize();

//     Eigen::Matrix3f TBN;
//     TBN.col(0) = t;
//     TBN.col(1) = b;
//     TBN.col(2) = normal;

//     float dU = kh * kn * (h1 - h0);
//     float dV = kh * kn * (h2 - h0);

//     Eigen::Vector3f ln(-dU, -dV, 1.0f);
//     ln.normalize();

//     normal = (TBN * ln).normalized();

//     Eigen::Vector3f result_color = {0, 0, 0};
//     for (auto& light : lights)
//     {
       
//         Eigen::Vector3f ambient = ka.cwiseProduct(amb_light_intensity);
        
//         auto i = light.intensity;
//         Eigen::Vector3f l = light.position - point;
//         float r_square = (l).dot(l);
        
//         Eigen::Vector3f h = ((eye_pos - point).normalized() + l.normalized());
//         float lambert_cosine = std::max(0.0f, normal.dot(l.normalized()));
//         float specular_cosine = std::max(0.0f, normal.dot(h.normalized()));
        
//         //diffuse
        
//         Eigen::Vector3f diffuse_color = kd.cwiseProduct(i / r_square) * lambert_cosine;
//         Eigen::Vector3f specular_color = ks.cwiseProduct(i / r_square) * std::pow(specular_cosine, p);
//         result_color += (ambient + diffuse_color + specular_color);
        
//     }

//     return result_color * 255.f;
