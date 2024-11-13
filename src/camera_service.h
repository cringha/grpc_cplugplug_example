#ifndef __CAMERA_SERVICE_H_
#define __CAMERA_SERVICE_H_

#include <string>
#include <map>
#include <vector>
#include "utils.h"
#include "camera-service-config.h"

#define DEFAULT_IMAGE_SIZE 20 * 1024*1024 



struct ImageParameter
{
    uint32_t imageType; // 1, bmp, 2, jpg 3 png , 4 tif 
    uint32_t width ; //0 
    uint32_t height; //0
    uint32_t quality; // 50-99 
    uint32_t method; // 0123

};

struct CameraMetaInfo
{
    std::string serialNumber ;
    std::string modelName  ;
    std::string manufactureName ;
    std::string deviceVersion ;
    std::string userDefinedName ;
    std::string cameraType ;
    std::map<std::string, std::string> info ;
};


class ICameraServiceProvider
{
public:

    virtual int enum_all_cameras(std::vector<CameraMetaInfo>& infos) = 0;
    
    virtual int take_picture( const std::string & sn , ImageParameter * param, buff_t* data_) = 0;
};


std::shared_ptr< ICameraServiceProvider> create_camera_service_provider(  CameraProviderConfig *  config);




#endif // __CAMERA_SERVICE_H_