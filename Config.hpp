#pragma once
#include <iostream>
#include <map>
#include <fstream>
#include "nlohmann/json.hpp"

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <limits.h>
#endif

std::string getCurrentDir() {
#ifdef _WIN32
    char buffer[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, buffer);
    return std::string(buffer);
#else
    char buffer[PATH_MAX];
    getcwd(buffer, sizeof(buffer));
    return std::string(buffer);
#endif
}

struct CameraConfig {
    Eigen::Vector3f eye;
    float fov;
    float znear;
    float zfar;
    float aspect;
};

struct RenderConfig {
    int width;
    int height ;
    std::string shader;
    Eigen::Vector3f amb_light_intensity;
};

struct ModelConfig {
    std::string obj;
    float angle;
};

struct TextureConfig {
    std::string model;
    std::string base_color;
    std::string height;
};

struct Config
{
    TextureConfig texture_config;
    CameraConfig camera_config;
    RenderConfig render_config;
    ModelConfig model_config;

    std::string configFilePath = getCurrentDir() + "/config/render_config.json";
    
    Config()
    {
        LoadFromFile();
    }
    

    bool LoadFromFile()
    {
        std::ifstream file(configFilePath);
        if (!file.is_open())
        {
            std::cout << "Config File Not Open: " << configFilePath << std::endl;
            return false;
        }

        // Load json file to ConfigMap

        nlohmann::json j;
        file >> j;

        // camera
        if (j.contains("camera")) {
                auto jc = j["camera"];
                 if (jc.contains("eye") && jc["eye"].is_array() && jc["eye"].size()==3) {
                                camera_config.eye = Eigen::Vector3f(jc["eye"][0].get<float>(),
                                                                    jc["eye"][1].get<float>(),
                                                                    jc["eye"][2].get<float>());
                                        }
                camera_config.fov    = jc.value("fov", 45.0f);
                camera_config.znear  = jc.value("znear", 0.1f);
                camera_config.zfar   = jc.value("zfar", 50.0f);
                camera_config.aspect = jc.value("aspect", 1.0f);
            }
        
        // texture
        if (j.contains("textures")) {
                auto jc = j["textures"];
                texture_config.base_color    = jc.value("base_color", "");
                texture_config.height    = jc.value("height", "");
                
            }
        
        // render
        if (j.contains("render")) {
                auto jr = j["render"];
                render_config.width  = jr.value("width", 100);
                render_config.height = jr.value("height", 100);
                render_config.shader = jr.value("shader", "texture_fragment_shader");
                if (jr.contains("amb_light_intensity") && jr["amb_light_intensity"].is_array() && jr["amb_light_intensity"].size()==3) {
                                render_config.amb_light_intensity = Eigen::Vector3f(jr["amb_light_intensity"][0].get<float>(),
                                                                    jr["amb_light_intensity"][1].get<float>(),
                                                                    jr["amb_light_intensity"][2].get<float>());
                }
            }

        // model
        if (j.contains("model")) {
                auto jc = j["model"];
                model_config.obj    = jc.value("obj", "");
                model_config.angle  = jc.value("angle", 140.0);
            }
        return true;
    }
    
};
