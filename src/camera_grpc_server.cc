#pragma warning(disable : 4099)

#include <iostream>
#include <memory>
#include <spdlog/spdlog.h>
#include <string>
#include <thread>
// #include "absl/flags/flag.h"
// #include "absl/flags/parse.h"
// #include "absl/strings/str_format.h"

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#include "camera-service.grpc.pb.h"
#include "camera_service.h"
#include "camera-service-config.h"

#include "utils.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

using service::grpc::BytesBody;
using service::grpc::CameraMetas;
using service::grpc::CameraService;
using service::grpc::ImageShape_;

const int MAX_BUFF = 1024 * 1024 * 3;

// Logic and data behind the server's behavior.
class CameraServiceImpl final : public CameraService::Service
{
public:

    ~CameraServiceImpl()
    {
       
    }

    void   dump_cameras()
    {
        try
        {
            std::vector<CameraMetaInfo> infos;
            int ret = camera_service_->enum_all_cameras(infos);
            if (ret)
            {
                spdlog::error("in enum_all_cameras   err : {:x} ", ret  );
                return  ;
            }
            spdlog::info ("in enum_all_cameras   get cameras {} ", infos.size());
            for (auto it = infos.begin(); it != infos.end(); it++)
            {
               
                spdlog::info("   sn: {}   {} {}  ", it->serialNumber, it->cameraType , it->modelName, it->manufactureName, it->deviceVersion);
                
            }
            return ;
        }
        catch (std::exception& e)
        {
            spdlog::error("in GetCameraMetas   err : {} ", e.what());
            
        }
    }

    Status GetCameraMetas(ServerContext* context,
        const ::google::protobuf::Empty* request,
        CameraMetas* response)
    {
        try
        {
            std::vector<CameraMetaInfo> infos;
            int ret = camera_service_->enum_all_cameras(infos);
            if (ret)
            {
            }

            for (auto it = infos.begin(); it != infos.end(); it++)
            {

                auto m = response->add_data();
                m->set_serialnumber(it->serialNumber);
                m->set_modelname(it->modelName);
                m->set_manufacturename(it->manufactureName);
                m->set_deviceversion(it->deviceVersion);
                m->set_userdefinedname(it->userDefinedName);
                m->set_cameratype(it->cameraType);

                if (!it->info.empty())
                {
                    for (auto ita = it->info.begin(); ita != it->info.end(); ita++)
                    {
                        m->mutable_info()->insert(std::make_pair(ita->first, ita->second));
                    }
                }
            }
            return Status::OK;
        }
        catch (std::exception& e)
        {
            spdlog::error("in GetCameraMetas   err : {} ", e.what());
            return Status::CANCELLED;
        }
    }
    // input camera sn , return 1 frame , alive image
    Status GetImage(ServerContext* context,
        const ::service::grpc::ImageRequest_* request,
        BytesBody* response)
    {
        try
        {
            std::string sn = request->sn();

            auto shape = request->imageshape();
            int32_t height = shape.height();
            int32_t width = shape.width();
            int32_t channel = shape.channel();

            spdlog::debug("call GetImage {} - {} {} {}  ", request->sn(), height,
                width, channel);

            ImageParameter param = { 0 };
            param.quality = 99;
            param.method = 3;
            param.height = height;
            param.width = width;

            buff_t data;

            data.ensure_size(DEFAULT_IMAGE_SIZE);

            int ret = camera_service_->take_picture(sn , NULL, &data);
            if (ret)
            {
            }

            response->set_content(data.get_buff(), data.get_data_len());

            return Status::OK;
        }
        catch (std::exception& e)
        {
            spdlog::error("in GetImage   err : {} ", e.what());
            return Status::CANCELLED;
        }
    }
    Status Reset(::grpc::ServerContext* context,
        const ::google::protobuf::Empty* request,
        ::google::protobuf::Empty* response)
    {

        return Status::OK;
    }

    int init_camera_service_provider(
        CameraProviderConfig* config)
    {
        camera_service_ = create_camera_service_provider(config);
        return 0;
    }

private:
    std::shared_ptr<ICameraServiceProvider> camera_service_;
};

class OnlyForShutdownGrpcServer
{
private:
    std::thread process_thread_;
    //std::unique_ptr<Server> server;
    grpc::Server* server = nullptr;
public:
    OnlyForShutdownGrpcServer(grpc::Server* s)
    {
        server = s;
    }
    void run()
    {
        loop_until();

        spdlog::debug("Get sigal , ready to shutdown grpc server");

        if (server)
        {
            server->Shutdown();
        }
    }

    int   start()
    {
        try
        {
            process_thread_ = std::thread(std::bind(&OnlyForShutdownGrpcServer::run, this));
            return 0;
        }
        catch (std::exception const& e)
        {
            spdlog::warn("WS run error  :{}", e.what());
            return -1;
        }
    }

    void wait()
    {
        process_thread_.join();
    }
};


void dump_metas()
{

}

void start_grpc_server(CameraServiceConfig* config)
{

    std::string v = grpc::Version();

    spdlog::info("grpc version: {} ", v.c_str());
    char url[1024];
    std::snprintf(url, sizeof(url), "%s:%d", config->service.host.c_str(), config->service.port);

    CameraServiceImpl service;
    service.init_camera_service_provider(&config->provider_config);

    service.dump_cameras();

    grpc::EnableDefaultHealthCheckService(true);
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();
    ServerBuilder builder;
    // Listen on the given address without any authentication mechanism.
    builder.AddListeningPort(url, grpc::InsecureServerCredentials());
    // Register "service" as the instance through which we'll communicate with
    // clients. In this case it corresponds to an *synchronous* service.
    builder.RegisterService(&service);
    // Finally assemble the server.
    std::unique_ptr<Server> server(builder.BuildAndStart());
    spdlog::info("Server listening on : {} ", url);

    OnlyForShutdownGrpcServer s(server.get());
    s.start();

    // Wait for the server to shutdown. Note that some other thread must be
    // responsible for shutting down the server for this call to ever return.
    server->Wait();
    s.wait();
    
}
