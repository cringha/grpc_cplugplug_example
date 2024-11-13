 
#include <cmdline.h>
#include <spdlog/spdlog.h>
#include "camera_grpc_service.h"
#include "camera_service.h"
#include "camera-service-config.h"
#include "version.h"
 
#define DEFAULT_CONFIG_FILE     "config.yaml"
#define DEFAULT_LOG_NAME  "camera-service.log"
void print_version()
{
	spdlog::info("=======================================================================");
	spdlog::info("");
	spdlog::info("");
	spdlog::info("Camera service (cpp) version : {} ", get_tag_version());
	spdlog::info("Program build timestamp : {} ", get_build_timestamp());
	spdlog::info("Git commit id           : {} ", get_commit_id());


}
//int main (int argc, char** argv)
//{
//	std::string a = "a3bGEREaac";
//	to_upper( a );
//
//	local_stricmp( "abc","Abc");
//
//	return 0;
//}

int main (int argc, char** argv)
{
 
 
    int ret = 0 ;
	std::string loglevel = "";
	std::string fileName = "";
	std::string logpath = "";

	bool console_exist = false;
	bool version_exist = false;
	try
	{
		cmdline::parser opt;

		opt.add<std::string>("conf", 'f', "config file", false, DEFAULT_CONFIG_FILE);
		opt.add<std::string>("log-path", 'p', "log file path", false, "");
		opt.add<std::string>("log-level", 'l', "log level [debug|info|warn|error|fatal|none]", false, "");
		opt.add("console", '\0', "log output console");
		opt.add("version", 'v', "display version");


		opt.parse_check(argc, argv);

		//zmq_port = opt.get<int >("zmq-port");
 
		loglevel = opt.get<std::string >("log-level");

		// load config file
		fileName = opt.get<std::string>("conf");

		logpath = opt.get<std::string>("log-path");

	 
		version_exist = opt.exist("version");
	 
		console_exist = opt.exist("console");
	 
 
	}
	catch (cmdline::cmdline_error& e)
	{
		spdlog::error ("params parse error: {} ", e.what());
		return -1;
	}

	print_version();
	if (version_exist)
	{
		return 0;
	}

 
	if (fileName == "")
	{
		spdlog::error("conf empty, please using `--conf` set config file ");
		return -1;
	}

	spdlog::info("Current: {}", get_current_path());  
	


	CameraServiceConfig config   ;
	
	ret = load_config(fileName, &config );
	if (ret)
	{
		return -1;
	}

    


	if (logpath == "")
	{
		logpath = config.logs.base;
	}
	if (logpath == "")
	{
		logpath="./logs";
	}


	init_logs(logpath, DEFAULT_LOG_NAME, "cs", console_exist);

	std::string loglevel_ = "";

	if (!loglevel.empty())
	{
		loglevel_ = loglevel;
	}
	else
	{
		loglevel_ =  config.logs.level;
	}

	set_log_level(loglevel_);


	init_loop_signal();

    start_grpc_server(&config);

    return 0;
}


