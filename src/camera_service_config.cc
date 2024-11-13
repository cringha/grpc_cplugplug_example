


#include <iostream>
#include <spdlog/spdlog.h>
#define YAML_CPP_STATIC_DEFINE 1
#include <yaml-cpp/yaml.h>

#include "camera-service-config.h"


void load_default_config(CameraServiceConfig* config)
{
    config->logs.base = "./logs";
    config->logs.level = "error";
    config->service.port = 45575;
    config->service.host = "localhost";
}


int read_provider_config(const YAML::Node& data, CameraProviderConfig* config)
{
    if (data)
    {

        YAML::Node port = data["port"];
        if (port)
        {
            //config->port = port.as<int>();
        }
        else
        {
            spdlog::warn("config: no websocket.port exsit ");
        }

    }
    else
    {
        spdlog::warn("config: no websocket node exsit ");
    }

    return 0;
}



int read_service_config(const YAML::Node& data, GrpcService* config)
{
    if (!data)
    {
        spdlog::warn("config: no camera service node exsit ");
        return 0;
    }

    YAML::Node port = data["port"];
    if (port)
    {
        config->port = port.as<int>();
    }
    else
    {
        spdlog::warn("config: no camera service.port exsit ");
    }


    YAML::Node host = data["host"];
    if (host)
    {
        config->host = host.as<std::string>();
    }
    else
    {
        spdlog::warn("config: no camera service.host exsit ");
    }


    return 0;
}

int read_logs_config(const YAML::Node& data, LogConfig* config)
{
    if (!data)
    {
        spdlog::warn("config: no logs node exsit ");
        return 0;
    }


    YAML::Node base = data["base"];
    if (base)
    {
        config->base = base.as<std::string>();
    }
    else
    {
        spdlog::warn("config: no config.base  exsit ");
    }

    YAML::Node level = data["level"];
    if (level)
    {
        config->level = level.as<std::string>();
    }
    else
    {
        spdlog::warn("config: no config.level  exsit ");
    }

    return 0;
}


int load_config(const std::string& filename, CameraServiceConfig* config)
{

    load_default_config(config);

    try
    {
        //  
        YAML::Node data = YAML::LoadFile(filename);
        YAML::Node camera_service = data["camera-service"];
        if (camera_service)
        {
            read_provider_config(camera_service["provider"], &config->provider_config);
            read_logs_config(camera_service["logs"], &config->logs);
            read_service_config(camera_service["service"], &config->service);
        }
        else
        {
            spdlog::warn("config: no `camera-service` exsit ");
        }
        return 0;

    }

    catch (YAML::Exception& e)
    {
        std::cout << "load config error " << e.what() << std::endl;
        return -1;
    }

    catch (std::exception& e)
    {
        std::cout << "load config error " << e.what() << std::endl;
        return -1;
    }


}