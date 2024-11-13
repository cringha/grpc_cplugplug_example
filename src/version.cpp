#include "version.h"  
 
const char * get_commit_id(){ 
 return "dcd48797d3ef946114dde476a1a2fd87584174b2"; 
} 
 
const char * get_build_timestamp(){ 
 return __TIMESTAMP__; 
} 
 
const char * get_tag_version(){ 
 return "v1.0.28"; 
} 
