
rem set PROTOC_CMD=D:\dev\gsu\project\protobuf-release-x64\protoc.exe
rem SET grpc_cpp_plugin="D:\dev\gsu\project\grpc-release-x64\grpc_cpp_plugin.exe"

set PROTOC_CMD=D:\dev\grpc\bin\protoc.exe
SET grpc_cpp_plugin="D:\dev\grpc\bin\grpc_cpp_plugin.exe"

SET source_proto_path=./grpc/proto
SET source_proto_file=%source_proto_path%/lidar-data.proto
set target_path=./grpc
%PROTOC_CMD% -I %source_proto_path% --grpc_out=%target_path% --plugin=protoc-gen-grpc=%grpc_cpp_plugin% %source_proto_file%
%PROTOC_CMD% -I %source_proto_path% --cpp_out=%target_path% %source_proto_file%


