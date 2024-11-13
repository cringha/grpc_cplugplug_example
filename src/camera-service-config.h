#ifndef _camera_service_config_h
#define _camera_service_config_h

#include <memory>
#include <string>



struct LogConfig
{
    std::string   base;
    std::string   level;
};
 

struct GrpcService
{
    std::string   host;
    int   port;
};

 

struct CameraProviderConfig
{

};



class CameraServiceConfig
{
public:
    GrpcService service;
    LogConfig    logs;
    CameraProviderConfig provider_config ;
};



int load_config(const std::string& filename, CameraServiceConfig* config);


#endif // _camera_service_config_h