#pragma warning( disable: 4099)

#include <iostream>
#include <memory>
#include <string>
#include <spdlog/spdlog.h>
//#include <strings.h>
#include "utils.h"
#include "camera_service.h"
#include "MvCamera.h"

//#define USE_GET_BUFF 1 
#define CARE_CAMERA_DEVICE_TYPE  MV_GIGE_DEVICE | MV_USB_DEVICE

#ifdef LOCAL_DEBUG

int   save_file(const char* filename, buff_t* data_)
{
    FILE* fp = NULL;
    int ret = fopen_s(&fp, filename, "wb");
    if (fp == NULL)
    {
        int err = errno;
        spdlog::error("open new file error , n ame:{}, err:{} ", filename, err);
        return -1;
    }


    int   data_len = data_->get_data_len();

    size_t len = fwrite(data_->get_buff(), 1, data_len, fp);
    if (len < data_len)
    {
        int err = errno;
        spdlog::error("save file err , len:{}, err:{} ", data_len, err);
        fclose(fp);
        return -1;
    }

    fclose(fp);
    return len;

}
#endif 



class CameraDevice
{
private:
    CMvCamera* camera_;
    bool        camera_open_;
    bool        grapping_;
    uint32_t    default_buff_size_;
    std::mutex  lock_;

    buff_t      cache_data_ ;


    uint64_t last_conv_image_ = 0;
    uint64_t total_conv_image_ = 0;
    uint64_t count_conv_image_ = 0;
    float    avg_conv_image_ = 0;

    uint64_t  last_take_picture_ = 0;
    uint64_t  total_take_picture_ = 0;
    float     avg_take_picture_ = .0;
    uint64_t  count_take_picture_ = 0;


    uint32_t    wait_timeout_ = 4000;
    

    MV_SAVE_IAMGE_TYPE default_image_type_  = MV_Image_Jpeg;
    uint32_t  default_quality = 99;
    uint32_t  default_method = 3;
public:

    CameraDevice()
    {
        camera_open_ = false ;
        grapping_ = false;
        default_buff_size_ = 0;
        camera_ = NULL;
    }
    ~CameraDevice()
    {
        stop_and_close();
    }


protected:






    int _to_jpg(unsigned char * buff ,  MV_FRAME_OUT_INFO_EX * frame, ImageParameter* param,  buff_t* data_)
    {
        /*  unsigned char* pData;                                  ///< [IN]  \~chinese 输入数据缓存           \~english Input Data Buffer
          unsigned int            nDataLen;                               ///< [IN]  \~chinese 输入数据长度           \~english Input Data length
          enum MvGvspPixelType    enPixelType;                            ///< [IN]  \~chinese 输入数据的像素格式     \~english Input Data Pixel Format
          unsigned int          nWidth;                                   ///< [IN]  \~chinese 图像宽                 \~english Image Width
          unsigned int          nHeight;                                  ///< [IN]  \~chinese 图像高                 \~english Image Height

          unsigned char* pImageBuffer;                           ///< [OUT] \~chinese 输出图片缓存           \~english Output Image Buffer
          unsigned int            nImageLen;                              ///< [OUT] \~chinese 输出图片长度           \~english Output Image length
          unsigned int            nBufferSize;                            ///< [IN]  \~chinese 提供的输出缓冲区大小   \~english Output buffer size provided
          enum MV_SAVE_IAMGE_TYPE enImageType;                            ///< [IN]  \~chinese 输出图片格式           \~english Output Image Format
          unsigned int            nJpgQuality;                            ///< [IN]  \~chinese JPG编码质量(50-99]，其它格式无效   \~english Encoding quality(50-99]，Other formats are invalid


          unsigned int            iMethodValue;                           ///< [IN]  \~chinese 插值方法 0-快速 1-均衡 2-最优（其它值默认为最优） 3-最优+  \~english Bayer interpolation method  0-Fast 1-Equilibrium 2-Optimal 3-Optimal+

          unsigned int            nReserved[3];                           ///<       \~chinese 预留                   \~english Reserved*/

        MV_SAVE_IMAGE_PARAM_EX3 msipe = { 0 };

        if (default_buff_size_ > 0)
        {
            data_->ensure_size(default_buff_size_);
        }
 
        uint32_t  width = frame->nWidth;
        uint32_t height = frame->nHeight;
        MV_SAVE_IAMGE_TYPE image_type = default_image_type_;
        unsigned int Quality = default_quality;
        unsigned int method = default_method;
        if (param)
        {
            if (param->height > 0)
            {
                height = std::min( height , param->height);
            }
            if (param->width > 0)
            {
                width = std::min(width, param->width);
            }
            if (param->imageType >= 1 && param->imageType <= 4)
            {
                image_type = (MV_SAVE_IAMGE_TYPE) param->imageType;
            }
            if (param->quality >= 50 && param->quality < 100)
            {
                Quality = param->quality;
            }
            if (param->method >= 0 && param->method <=3 )
            {
                method = param->method;
            }
        }

        msipe.pData = buff ;
        msipe.nDataLen = frame->nFrameLen;
        msipe.enPixelType = frame->enPixelType;
        msipe.nWidth = width ;
        msipe.nHeight = height ;
        msipe.enPixelType = frame->enPixelType;

        msipe.pImageBuffer = data_->get_buff();
        msipe.nBufferSize = data_->get_buff_size();


        msipe.enImageType = image_type;
        msipe.nJpgQuality = Quality ;
        msipe.iMethodValue = method;


        uint64_t t1 = get_sys_time_ms();
        int ret = camera_->SaveImage(&msipe);
        uint64_t t2 = get_sys_time_ms();
        last_conv_image_  = t2 - t1;
        count_conv_image_ += 1;
        total_conv_image_ += last_conv_image_;
        avg_conv_image_ = total_conv_image_ / count_conv_image_;
        
        spdlog::debug  ("in SaveImage, w:{} , h:{} ,  return {:#x} , data length : {} , off:{} , avg-conv : {}   ", width, height, (uint32_t) ret , msipe.nImageLen, last_conv_image_, avg_conv_image_ );

        if (ret == 0)
        {
            data_->set_data_size(msipe.nImageLen);
            //  save_file( "d:/test.jpg", data_ );
        }

        return ret;
    }


    int _start_grapping()
    {
        if (grapping_)
        {
            return 0;
        }

    

        int ret = camera_->StartGrabbing();
        spdlog::info ("in start_grapping, start  {:#x}  ", (uint32_t)ret);
        if (ret)
        {
            spdlog::error("in start_grapping, error {:#x}  ", (uint32_t)ret);
            return ret;
        }
        grapping_ = true;
        return 0;
    }
    int _stop_grapping()
    {

        if (grapping_)
        {
            int times = 0;
        __retry:
            int ret = camera_->StopGrabbing();
            if (ret)
            {
                spdlog::error("in start_grapping, error {:#x}  ", (uint32_t)ret);
                if (ret != MV_E_USB_WRITE) // use device dis connected ,
                {   
                    if (times < 5)
                    {
                        times += 1;
                        local_sleep(100);
                        goto __retry;
                    }

                }

            }
            grapping_ = false;
            return ret;
        }


        return 0;
    }


    int _close_camera()
    {
        camera_open_ = false;
        if (camera_)
        {
            camera_->Close();
            delete camera_;
            camera_ = NULL;
        }

        return 0;
    }
 

   
    // ch:按下打开设备按钮：打开设备 | en:Click Open button: Open Device
    int  _open_camera(MV_CC_DEVICE_INFO* device_info)
    {
 

        if (camera_open_ || NULL != camera_)
        {
            return 0;
        }
        // ch:由设备信息创建设备实例 | en:Device instance created by device information

        camera_ = new CMvCamera();
        if (NULL == camera_)
        {
            spdlog::error("in open_camera, cant new CMvCamera  ");
            return -1;
        }

        int ret = camera_->Open(device_info);
        if (MV_OK != ret)
        {
            delete camera_;
            camera_ = NULL;
            spdlog::error("in open_camera, open camera error, {:#x} ", (uint32_t)ret);
            return ret;
        }
        spdlog::info ("in open_camera, open camera   " );
        // ch:探测网络最佳包大小(只对GigE相机有效) | en:Detection network optimal package size(It only works for the GigE camera)
        if (device_info->nTLayerType == MV_GIGE_DEVICE)
        {
            unsigned int nPacketSize = 0;
            ret = camera_->GetOptimalPacketSize(&nPacketSize);

            spdlog::info("in open_camera, GetOptimalPacketSize  {:#x} , {}  ", (uint32_t)ret , nPacketSize );


            if (ret == MV_OK)
            {
                ret = camera_->SetIntValue("GevSCPSPacketSize", nPacketSize);
                if (ret != MV_OK)
                {
                    spdlog::warn("in open_camera, Set Packet Size fail!, new package {} , ret {:#x}", nPacketSize, (uint32_t)ret);
                }
            }
            
            else
            {
                spdlog::warn("in open_camera,Get Packet Size fail!,  ret {:#x}", (uint32_t)ret);
            }
        }

        _init_cam_params();
      
        camera_open_ = true;
        return 0;
    }

    int _stop_and_close()
    {
     
        _stop_grapping();
        _close_camera();
        return 0;
    }

    


    int _init_cam_params()
    {
        int ret= camera_->SetEnumValue("TriggerMode", MV_TRIGGER_MODE_OFF);
        spdlog::info ("in _init_cam_params ,ser TriggerMode MV_TRIGGER_MODE_OFF  {:#x}", (uint32_t)ret);

        ret = camera_->SetEnumValue("PixelFormat", PixelType_Gvsp_BGR8_Packed);
        spdlog::info("in _init_cam_params ,ser PixelFormat PixelType_Gvsp_BGR8_Packed  {:#x}", (uint32_t)ret);
        
        MVCC_INTVALUE_EX it={0};
        spdlog::info("in _init_cam_params ,before get  PayloadSize  " );
        ret = camera_->GetIntValue("PayloadSize", &it );
        //printf(" GetIntValue PayloadSize %d , %uld\n", (uint32_t)ret, it.nCurValue );
        spdlog::info("in _init_cam_params , get PayloadSize  ret {:#x}, cur: {} ", (uint32_t)ret,  (unsigned int ) it.nCurValue);
        
        if (ret == 0 )
        {
            default_buff_size_ = it.nCurValue;
        }else {
            spdlog::info("in _init_cam_params ,get PayloadSize  ret {:#x}   ", (uint32_t)ret );
        }
        return ret;
    }

public:

    bool  is_shutdown()
    {
        std::lock_guard<std::mutex> lg(lock_);
        return camera_ == NULL;
    }



    int take_picture(ImageParameter* param,  buff_t* data_)
    {
        std::lock_guard<std::mutex> lg(lock_);

        assert(camera_ != NULL);
        assert(camera_open_);
        assert(grapping_);

        bool  connected = camera_->IsDeviceConnected();
        if (!connected)
        {
            spdlog::error("in IsDeviceConnected   : {} ", connected);
            _stop_and_close();
            return -1;
        }
        spdlog::debug ("in take_picture , camera connected ");
 
        uint64_t t1 = get_sys_time_ms();
        
#ifdef USE_GET_BUFF 
        MV_FRAME_OUT stImageInfo = { 0 };
        int    ret = camera_->GetImageBuffer(&stImageInfo, wait_timeout_);
        int frameLength = stImageInfo.stFrameInfo.nFrameLen; 
         
#else

        spdlog::debug ("in take_picture , default_buff_size_ {} ", default_buff_size_ );
        if (default_buff_size_ > 0)
        {
            cache_data_.ensure_size(default_buff_size_);
        }
        MV_FRAME_OUT_INFO_EX  stImageInfo = {0};
        int    ret = camera_->GetOneFrameTimeout(cache_data_.get_buff(), cache_data_.get_buff_size(), & stImageInfo , wait_timeout_);
        int frameLength = stImageInfo.nFrameLen ;
        spdlog::debug("in take_picture , GetOneFrameTimeout   {:#x} ", (uint32_t)ret );
        
#endif 
        uint64_t t2 = get_sys_time_ms();
        last_take_picture_ = t2 - t1 ;
        count_take_picture_ += 1;
        total_take_picture_ += last_take_picture_ ;
        avg_take_picture_ = total_take_picture_ / count_take_picture_;
        
        spdlog::debug("in take_picture, return {:#x} , data length : {} , avg-conv : {:+f}   ", (uint32_t)ret, (uint32_t) frameLength, avg_take_picture_);


        if (ret)
        {
            spdlog::error("in GetImageBuffer err : {:#x} ", (uint32_t)ret);
            if (ret == MV_E_NODATA)
            {
                _stop_and_close(); // usb device dis connected 
            }
            return ret;
        }
#ifdef USE_GET_BUFF 
        ret = _to_jpg(stImageInfo.pBufAddr, &stImageInfo.stFrameInfo, param, data_);
#else
        ret = _to_jpg(cache_data_.get_buff(), &stImageInfo, param, data_);
#endif


#ifdef USE_GET_BUFF 
        camera_->FreeImageBuffer(&stImageInfo);
#endif 
        return ret;

    }

    int  start(MV_CC_DEVICE_INFO* device_info)
    {
        std::lock_guard<std::mutex> lg(lock_);
        int ret = _open_camera(device_info);
        if (ret)
        {
            _close_camera();
            return ret;
        }


        return _start_grapping();
    }
 

    int stop_and_close()
    {
        std::lock_guard<std::mutex> lg(lock_);
        _stop_and_close();
        return 0;
    }




};

class CameraServiceProvider: public ICameraServiceProvider
{
private:

    std::mutex  lock_camera_info_list_;
    MV_CC_DEVICE_INFO_LIST  camera_info_list_;

    std::mutex  lock_cameras_list_;
    std::map<std::string, std::shared_ptr< CameraDevice> > cameras_list_;
    
    CameraProviderConfig * config;

protected:

public:

    ~CameraServiceProvider()
    {

    }

    CameraServiceProvider(CameraProviderConfig * config1 )
    {
        std::lock_guard<std::mutex> lg(lock_camera_info_list_);
        memset((void*)&camera_info_list_, 0, sizeof(camera_info_list_));
        config = config1;
    }


    MV_CC_DEVICE_INFO* find_camera_info_by_sn(const char* sn)
    {
        std::lock_guard<std::mutex> lg(lock_camera_info_list_);
        int sn_len = strlen(sn);
        for (unsigned int i = 0; i < camera_info_list_.nDeviceNum; i++)
        {
            MV_CC_DEVICE_INFO* pDeviceInfo = camera_info_list_.pDeviceInfo[i];
            if (NULL == pDeviceInfo)
            {
                continue;
            }
            if (pDeviceInfo->nTLayerType == MV_GIGE_DEVICE)
            {
                if (local_stricmp((char *)pDeviceInfo->SpecialInfo.stGigEInfo.chSerialNumber, sn ) == 0)
                {
                    return pDeviceInfo;
                }
            }
            else if (pDeviceInfo->nTLayerType == MV_USB_DEVICE)
            {
                if (local_stricmp((char*)pDeviceInfo->SpecialInfo.stUsb3VInfo.chSerialNumber, sn ) == 0)
                {
                    return pDeviceInfo;
                }
            }
        }
        return NULL;
    }

    void close_camera(const char* sn)
    {
         
        auto find = find_camera_device_by_sn(sn);
        if (find)
        {
            find->stop_and_close();
        }
        
    }
    std::shared_ptr< CameraDevice> find_camera_device_by_sn(const std::string & sn)
    {
        {
            std::lock_guard<std::mutex> lg(lock_cameras_list_);

            //find in map 
            auto find = cameras_list_.find(sn);
            if (find != cameras_list_.end())
            {
                // device handle has closed , remove from 
                if (find->second->is_shutdown())
                {
                    cameras_list_.erase(sn);
                    return nullptr;
                }

                return find->second;
            }
            return nullptr;
        }

    }

    void  add_camera_device(const std::string sn, std::shared_ptr< CameraDevice> a)
    {
        {
            std::lock_guard<std::mutex> lg(lock_cameras_list_);
            cameras_list_[sn] = a;
        }
    }
    

    std::shared_ptr< CameraDevice>   find_or_create_camera_device(const std::string & sn1)
    {
 
        std::string sn = sn1 ;
        to_upper( sn );

        auto find = find_camera_device_by_sn(sn); 
        if (find)
        {
            return find ;
        }

        // or create new one 
       
        MV_CC_DEVICE_INFO* info = find_camera_info_by_sn(sn.c_str());
        if (info == NULL)
        {
            spdlog::error("in create_camera_device can't find camera by sn: {} ", sn);
            return nullptr;
        }

        std::shared_ptr< CameraDevice> a = std::make_shared<CameraDevice>();
        int ret = a->start(info);
        if (ret)
        {
            spdlog::error("in create_camera_device open camera err : {} ", ret);
            return nullptr;
        }

        spdlog::info ("in create_camera_device add camera `{}` to map ", sn );
       
        add_camera_device( sn, a );

        return a;

    }
    int enum_all_cameras_info ()
    {
        std::lock_guard<std::mutex> lg(lock_camera_info_list_);

        // MV_CC_DEVICE_INFO_LIST  camera_list;

        // ch:枚举子网内所有设备 | en:Enumerate all devices within subnet
        int ret = CMvCamera::EnumDevices(CARE_CAMERA_DEVICE_TYPE, &camera_info_list_);
        if (MV_OK != ret)
        {
            spdlog::error("in EnumDevices err : {0:x} ", (uint32_t)ret);
            return ret;
        }
        return ret ;
        
    }

    int enum_all_cameras(std::vector<CameraMetaInfo>& infos)
    {
        int ret = enum_all_cameras_info();
        if (ret)
        {
            return ret;
        }
        return to_camera_meta_info_list( &camera_info_list_ , infos );
    }
    
    static 
    int to_camera_meta_info_list(MV_CC_DEVICE_INFO_LIST * camera_info_list_, std::vector<CameraMetaInfo>& infos)
    {

        

        // ch:将值加入到信息列表框中并显示出来 | en:Add value to the information list box and display
        for (unsigned int i = 0; i < camera_info_list_->nDeviceNum; i++)
        {
            MV_CC_DEVICE_INFO* pDeviceInfo = camera_info_list_->pDeviceInfo[i];
            if (NULL == pDeviceInfo)
            {
                continue;
            }

            char strUserName[256] = { 0 };
            wchar_t* pUserName = NULL;
            if (pDeviceInfo->nTLayerType == MV_GIGE_DEVICE)
            {

                CameraMetaInfo info;

                int nIp1 = ((pDeviceInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0xff000000) >> 24);
                int nIp2 = ((pDeviceInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0x00ff0000) >> 16);
                int nIp3 = ((pDeviceInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0x0000ff00) >> 8);
                int nIp4 = (pDeviceInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0x000000ff);

                info.cameraType = "gige";

                if (pDeviceInfo->SpecialInfo.stGigEInfo.chUserDefinedName)
                {
                    info.userDefinedName = (const char*)(pDeviceInfo->SpecialInfo.stGigEInfo.chUserDefinedName);
                }
                if (pDeviceInfo->SpecialInfo.stGigEInfo.chSerialNumber)
                {
                    info.serialNumber = (const char*)pDeviceInfo->SpecialInfo.stGigEInfo.chSerialNumber;
                }

                if (pDeviceInfo->SpecialInfo.stGigEInfo.chModelName)
                {
                    info.modelName = (const char*)pDeviceInfo->SpecialInfo.stGigEInfo.chModelName;
                }


                if (pDeviceInfo->SpecialInfo.stGigEInfo.chDeviceVersion)
                {
                    info.deviceVersion = (const char*)pDeviceInfo->SpecialInfo.stGigEInfo.chDeviceVersion;
                }


                if (pDeviceInfo->SpecialInfo.stGigEInfo.chManufacturerName)
                {
                    info.manufactureName = (const char*)pDeviceInfo->SpecialInfo.stGigEInfo.chManufacturerName;
                }
                {
                    char ipaddr[256];
                    snprintf(ipaddr, sizeof(ipaddr), "%d.%d.%d.%d", nIp1, nIp2, nIp3, nIp4);
                    info.info["ipAddr"] = ipaddr;
                }

                infos.push_back(info);

            }
            else if (pDeviceInfo->nTLayerType == MV_USB_DEVICE)
            {


                CameraMetaInfo info;


                info.cameraType = "usb";

                if (pDeviceInfo->SpecialInfo.stUsb3VInfo.chUserDefinedName)
                {
                    info.userDefinedName = (const char*)(pDeviceInfo->SpecialInfo.stUsb3VInfo.chUserDefinedName);
                }
                if (pDeviceInfo->SpecialInfo.stUsb3VInfo.chSerialNumber)
                {
                    info.serialNumber = (const char*)pDeviceInfo->SpecialInfo.stUsb3VInfo.chSerialNumber;
                }

                if (pDeviceInfo->SpecialInfo.stUsb3VInfo.chModelName)
                {
                    info.modelName = (const char*)pDeviceInfo->SpecialInfo.stUsb3VInfo.chModelName;
                }


                if (pDeviceInfo->SpecialInfo.stUsb3VInfo.chDeviceVersion)
                {
                    info.deviceVersion = (const char*)pDeviceInfo->SpecialInfo.stUsb3VInfo.chDeviceVersion;
                }


                if (pDeviceInfo->SpecialInfo.stUsb3VInfo.chManufacturerName)
                {
                    info.manufactureName = (const char*)pDeviceInfo->SpecialInfo.stUsb3VInfo.chManufacturerName;
                }
                if (pDeviceInfo->SpecialInfo.stUsb3VInfo.chDeviceGUID)
                {
                    info.info["deviceGUID"] = (const char*)pDeviceInfo->SpecialInfo.stUsb3VInfo.chDeviceGUID;

                }

                infos.push_back(info);
            }

            else
            {
                spdlog::error("in enum_all_cameras unknown camera type {} ", pDeviceInfo->nTLayerType);
            }

        }

        return 0;
    }
    int take_picture(const std::string &sn, ImageParameter* param,  buff_t* data_   )
    {

        

        auto camera = find_or_create_camera_device( sn );
        if (!camera)
        {
            spdlog::error("cant find or create device  {} ", sn );
            return -1;
        }


        spdlog::debug("in take_picture find camera `{}` ", sn);
 
        return camera->take_picture( param, data_ );
    }


};

std::shared_ptr< ICameraServiceProvider> create_camera_service_provider(CameraProviderConfig*  config )
{
    return std::make_shared< CameraServiceProvider> (config);
}


#define DEFAULT_IMAGE_SIZE 20 * 1024*1024 
#ifdef LOCAL_DEBUG
int main2(int argc, char** argv)
{
    std::vector<CameraMetaInfo> infos;
    CameraServiceProvider a(nullptr);
    a.enum_all_cameras(infos);

    for (auto it = infos.begin(); it != infos.end(); it++)
    {
        printf(" camera %s %s  %s \n", it->serialNumber.c_str(), it->cameraType.c_str(), it->modelName.c_str());
        if (!it->info.empty())
        {
            for (auto ita = it->info.begin(); ita != it->info.end(); ita++)
            {
                printf("      %s %s  \n", ita->first.c_str(), ita->second.c_str());

            }
        }
    }

    spdlog::set_level( spdlog::level::debug);

    ImageParameter param = {0};
    param.quality = 99;
    param.method = 3 ;

    buff_t data;

    data.ensure_size(DEFAULT_IMAGE_SIZE);

    const char * camera_sn = "00G74701273";

    for (int i = 0; i < 10; i++)
    {
        uint64_t t1 = get_sys_time_ms();
        int ret = a.take_picture(camera_sn, &param, &data);
        uint64_t t2 = get_sys_time_ms();

        uint64_t off = t2-t1;

        printf(" take_picture return %d, %d \n", ret, off );

        
        if (ret == 0)
        {
            char  filename[1024];

            sprintf_s (filename, "d:/%s_%d.jpg", camera_sn, i );

            save_file(filename, &data );

        }

        local_sleep(200);
    }

    return 0;
}
#endif 