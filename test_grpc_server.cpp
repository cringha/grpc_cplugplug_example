

#include <iostream>
#include <memory>
#include <string>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/strings/str_format.h"

  #include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

 
#include "lidar-data.grpc.pb.h"
 

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
//using helloworld::Greeter;
//using helloworld::HelloReply;
//using helloworld::HelloRequest;


using lidar::collector::DataService;
using lidar::collector::DataRequest_;
using lidar::collector::DataResult_;

const int  MAX_BUFF = 1024*1024*3;
ABSL_FLAG(uint16_t, port, 50051, "Server port for the service");

// Logic and data behind the server's behavior.
class DataServiceImpl final : public DataService::Service
{
    Status GetLidarData( ServerContext* context, 
            const  DataRequest_* request,  DataResult_* response)
    {
        response->set_status(0);
        response->set_message("");
        response->set_details("");

        unsigned char *  buff =  (unsigned char* )malloc(MAX_BUFF );
        for (int i = 0; i < MAX_BUFF; i++)
        {
            buff[i] = (unsigned char ) i ;
        }

     //   std::string a(buff) ;
        
        response->set_data(buff, MAX_BUFF );
        free(buff);
        return Status::OK;
    }
    //Status SayHello(ServerContext* context, const HelloRequest* request,
    //    HelloReply* reply) override
    //{
    //    std::string prefix("Hello ");
    //    reply->set_message(prefix + request->name());
    //    return Status::OK;
    //}
};

void RunServer(uint16_t port)
{

    std::string v = grpc::Version();

    printf("grpc version %s\n",  v.c_str());

    std::string server_address = absl::StrFormat("0.0.0.0:%d", port);
    DataServiceImpl service;

    grpc::EnableDefaultHealthCheckService(true);
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();
    ServerBuilder builder;
    // Listen on the given address without any authentication mechanism.
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    // Register "service" as the instance through which we'll communicate with
    // clients. In this case it corresponds to an *synchronous* service.
    builder.RegisterService(&service);
    // Finally assemble the server.
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;

    // Wait for the server to shutdown. Note that some other thread must be
    // responsible for shutting down the server for this call to ever return.
    server->Wait();
}

int main(int argc, char** argv)
{
    absl::ParseCommandLine(argc, argv);
    RunServer(absl::GetFlag(FLAGS_port));
    return 0;
}
