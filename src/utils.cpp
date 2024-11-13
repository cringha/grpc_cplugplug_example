#include <cstdint>
#include <cstring>
#include <signal.h>


#include <stdlib.h>
#include <time.h>

#include <spdlog/spdlog.h>

#ifdef _WIN32
#include <io.h>
#include <windows.h>

#define access _access_s
#else
#include <unistd.h>
#endif 

#include "utils.h"


#if !_HAS_CXX17

//bool FileExists(const std::string &Filename) {
//  return access(Filename.c_str(), 0) == 0;
//}

//inline bool exists_test0(const std::string &name) {
//  std::ifstream f(name.c_str());
//  return f.good();
//}

//inline bool exists_test1(const std::string &name) {
//  if (FILE *file = fopen(name.c_str(), "r")) {
//    fclose(file);
//    return true;
//  } else {
//    return false;
//  }
//}

//inline bool exists_test2(const std::string &name) {
//#ifndef F_OK
//#define F_OK 0
//#endif
//  return (access(name.c_str(), 0) != -1);
//}

inline bool file_exist(const std::string& name)
{
    struct stat buffer;
    return (stat(name.c_str(), &buffer) == 0);
}

#ifdef _WIN32 
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING 1
#include <experimental/filesystem>

std::string get_current_path()
{
    auto p1_ = std::experimental::filesystem::current_path();
       return p1_.string(); 

   

}




std::string path_join(const std::string& p1, const std::string p2)
{
    std::experimental::filesystem::path p1_ =
        std::experimental::filesystem::path(p1);
    p1_.append(p2);
    return p1_.string();
}


int create_directory(const std::string& path)
{
    std::error_code ec;
    ;
    std::experimental::filesystem::create_directories(path, ec);
    if (!ec)
    {
        return ec.value();
    }
    return 0;
}


bool mkpath_if_not_exist(const std::string& logpath)
{
    std::experimental::filesystem::path logpath_ = std::experimental::filesystem::path(logpath);
    if (!std::experimental::filesystem::exists(logpath_))
    {
        std::error_code ec;

        if (!std::experimental::filesystem::create_directories(logpath_, ec))
        {
            spdlog::warn("create_directories path err, {} , {}. {}.", logpath,
                ec.value(), ec.message());
            return false;
        }
    }

    return true;
}
#else 

    #ifdef _WIN32

    #else 

    std::string get_current_path()
    {
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != NULL)
        {
            return std::string(cwd);
        }
        else
        {
            return "";
        }
    }



    bool mkpath_if_not_exist(const std::string& logpath)
    {
     
        if (!file_exist(logpath))
        {
            std::error_code ec;
            int ret = mkdir(logpath.c_str(), 0700);
            if (ret == -1)
            {
                printf("create_directories path err, %s  %d\n", logpath.c_str(),
                     errno );
                return false;
            }
        }

        return true;
    }


    #ifdef _WIN32
    const char sep = '\\';
    #else 
    const char sep = '/';
    #endif


    std::string path_join(const std::string& p1, const std::string p2)
    {
        std::string tmp = p1;
        // Add separator if it is not included in the first path:
        if (p1[p1.length() - 1] != sep)
        {
            tmp += sep;
            return tmp + p2;
        }
        else
        {
            return p1 + p2;
        }
    }

    #endif 

#endif 
 

#else

bool file_exist(const std::string& logpath)
{
    std::filesystem::path logpath_ = std::filesystem::path(logpath);
    return std::filesystem::exists(logpath_);
}

bool mkpath_if_not_exist(const std::string& logpath)
{
    std::filesystem::path logpath_ = std::filesystem::path(logpath);
    if (!std::filesystem::exists(logpath_))
    {
        std::error_code ec;

        if (!std::filesystem::create_directories(logpath_, ec))
        {
            spdlog::warn("create_directories path err, {} , {}. {}.", logpath,
                ec.value(), ec.message());
            return false;
        }
    }

    return true;
}

std::string path_join(const std::string& p1, const std::string p2)
{
    std::filesystem::path p1_ = std::filesystem::path(p1);
    p1_.append(p2);
    return p1_.string();
}

std::string get_current_path()
{
    auto p1_ = std::filesystem::current_path();
    return p1_.string();
}

int create_directory(const std::string& path)
{
    std::error_code ec;
    ;
    std::filesystem::create_directories(path, ec);
    if (!ec)
    {
        return ec.value();
    }
    return 0;
}

#endif



// uint64_t get_sys_tick()
// {
// 	return GetTickCount();
// auto currentTime = std::chrono::system_clock::now();
// auto currentTime_ms =
// std::chrono::time_point_cast<std::chrono::milliseconds>(currentTime); auto
// currentTime_micro =
// std::chrono::time_point_cast<std::chrono::microseconds>(currentTime); return
// (uint64_t)currentTime_ms.time_since_epoch().count();
// }

uint64_t get_sys_time_ms()
{

    auto currentTime = std::chrono::system_clock::now();
    auto currentTime_ms =
        std::chrono::time_point_cast<std::chrono::milliseconds>(currentTime);
    // auto currentTime_micro =
    // std::chrono::time_point_cast<std::chrono::microseconds>(currentTime);
    return (uint64_t)currentTime_ms.time_since_epoch().count();
}
// std::size("yyyy-mm-ddThh:mm:ssZ")
std::string ts_to_string(uint64_t ts)
{
    std::chrono::system_clock::time_point st =
        std::chrono::system_clock::time_point(std::chrono::milliseconds(ts));
    std::time_t t = std::chrono::system_clock::to_time_t(st);
    char ts1[256];
    memset(ts1, 0, sizeof(ts1));
    struct tm tm_;
#ifdef _WIN32
    gmtime_s(&tm_, &t);
#else
    // struct tm *gmtime_r(const time_t *timep, struct tm *result);
    gmtime_r(&t, &tm_);
#endif 
    std::strftime(ts1, sizeof(ts1), "%F %T.", &tm_);
    return std::string(ts1) + std::to_string(ts % 1000);
    // return std::put_time(std::localtime(&t), "%c")
    /* string TimeConversion::GetISO8601TimeStringFrom(const milliseconds& ms)
       {
           return date::format("%FT%T%z", date::sys_time<milliseconds>{ ms });
       }*/
}


#ifdef USE_EVENT_TS

event_t::event_t()
{
    ev_ = NULL;
    create();
}
event_t::~event_t() { close(); }
int event_t::create()
{
    HANDLE ev = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (ev == NULL)
    {
        int code = GetLastError();
        spdlog::error("CreateEvent error ,error={}, message:{}", code,
            get_error_message(code));
        return code;
    }
    ev_ = ev;
    return 0;
}
int event_t::close()
{
    if (ev_ != NULL)
    {
        CloseHandle(ev_);
        ev_ = NULL;
    }
    return 0;
}
int event_t::wait(uint32_t ms)
{
    if (ev_ == NULL)
    {
        int e = create();
        if (e != 0)
        {
            return e;
        }
    }
    DWORD ret = WaitForSingleObject(ev_, ms);
    // ignore ret
    switch (ret)
    {
        case WAIT_TIMEOUT:
            return 0;
        case WAIT_OBJECT_0:
            return 0;
        case WAIT_FAILED:
        {
            int code = GetLastError();
            spdlog::error("WaitForSingleObject error ,error={}, message:{}", code,
                get_error_message(code));
            return code;
        }
    }
    return 0;
}

event_t g_event;

#endif


void local_sleep(uint32_t ms)
{
    if (ms <= 0)
    {
        return;
    }
#ifdef _WIN32
    // Sleep(ms);
    // #ifdef USE_TIME_PERIOD
    //   timeBeginPeriod(1); // ���þ���Ϊ1����
    // #endif
    ::Sleep(ms); // ��ǰ�̹߳���һ����
    // #ifdef USE_TIME_PERIOD
    //   timeEndPeriod(1);
    // #endif
#else
    // std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    usleep(ms * 1000);
#endif
}

status_t::status_t() { status_ = false; }
status_t::~status_t() {}

bool status_t::is_stopped()
{
    std::lock_guard<std::mutex> lg(mtx_);
    return status_;
}
void status_t::stop()
{
    std::lock_guard<std::mutex> lg(mtx_);
    status_ = true;
    return;
}

void status_t::loop()
{
    while (true)
    {
        {
            if (status_)
            {
                return;
            }
        }
        local_sleep(1000);
    }
}

status_t MAIN_PROGRAM_LOOP;

void loop_until() { MAIN_PROGRAM_LOOP.loop(); }

bool is_continue_loop() { return !MAIN_PROGRAM_LOOP.is_stopped(); }

void stop_loop() { MAIN_PROGRAM_LOOP.stop(); }

void signal_handler(int signum)
{
#ifdef USE_SPDLOG
    spdlog::info("recv signal `{}` to stop ", signum);
#endif
    MAIN_PROGRAM_LOOP.stop();
    // std::this_thread::sleep_for(std::chrono::seconds(5));
}

void init_loop_signal() { signal(SIGINT, signal_handler); }

#ifdef _WIN32

std::string get_error_message(unsigned int error_code)
{
    std::string error_str_info;
    char error_str[2048];
    error_str[0] = '\0';
    ::FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, NULL, error_code, LANG_NEUTRAL,
        error_str, sizeof(error_str) / sizeof(char), NULL);
    error_str_info.append(error_str);
    return error_str_info;
}

#endif

void init_rand() { srand(time(NULL)); }
int rand_int() { return rand(); }
int rand_int_r(int min_num, int max_num)
{
    int result = 0, low_num = 0, hi_num = 0;

    if (min_num < max_num)
    {
        low_num = min_num;
        hi_num = max_num + 1; // include max_num in output
    }
    else
    {
        low_num = max_num + 1; // include max_num in output
        hi_num = min_num;
    }

    result = (rand() % (hi_num - low_num)) + low_num;
    return result;
}
float rand_float_r(float min_num, float max_num)
{
    float result = 0, low_num = 0, hi_num = 0;

    if (min_num < max_num)
    {
        low_num = min_num;
        hi_num = max_num; // include max_num in output
    }
    else
    {
        low_num = max_num; // include max_num in output
        hi_num = min_num;
    }

    float x = (float)rand() / (float)(RAND_MAX / (hi_num - low_num));

    result = x + low_num;
    return result;
}

void hex_dump(unsigned char* buffer, int length)
{

    int c = 0;
    int enable = 0;
    int i, count, index;
    char rgbDigits[] = "0123456789abcdef";
    char rgbLine[100];
    char cbLine;

    for (c = 0, index = 0; length;
        c++, length -= count, buffer += count, index += count)
    {
        count = (length > 16) ? 16 : length;

        snprintf(rgbLine, sizeof(rgbLine) - 1, "%8.8x  ", c);
        //	sprintf(rgbLine, "\t\t\t");
        cbLine = 9;

        for (i = 0; i < count; i++)
        {
            rgbLine[cbLine++] = (char)rgbDigits[buffer[i] >> 4];
            rgbLine[cbLine++] = (char)rgbDigits[buffer[i] & 0x0f];
            if (i == 7)
            {
                rgbLine[cbLine++] = (char)' ';
            }
            else
            {
                rgbLine[cbLine++] = (char)' ';
            }
        }
        for (; i < 16; i++)
        {
            rgbLine[cbLine++] = (char)' ';
            rgbLine[cbLine++] = (char)' ';
            rgbLine[cbLine++] = (char)' ';
        }

        rgbLine[cbLine++] = (char)' ';

        for (i = 0; i < count; i++)
        {
            if (buffer[i] < 32 || buffer[i] > 126)
            {
                rgbLine[cbLine++] = (char)'.';
            }
            else
            {
                rgbLine[cbLine++] = (char)buffer[i];
            }
        }

        rgbLine[cbLine++] = (char)0;

        fprintf(stdout, "%s\n", rgbLine);
    }
}

buff_t::buff_t()
{
    buff_ = NULL;
    buff_max_size_ = 0;
    data_size_t_ = 0;
}

buff_t::~buff_t() { free_buff(); }
bool buff_t::is_empty()
{
    if (this->buff_ == NULL)
    {
        return true;
    }
    if (data_size_t_ == 0)
    {
        return true;
    }
    return false;
}
void buff_t::empty_buff()
{
    if (buff_ != NULL)
    {
        memset(buff_, 0, buff_max_size_);
    }
    data_size_t_ = 0;
}

uint32_t buff_t::set_data_size(uint32_t size)
{
    assert(size < buff_max_size_);
    uint32_t old = data_size_t_;
    data_size_t_ = size;
    return old;
}
int buff_t::set_byte(uint32_t pos, unsigned char val)
{
    if (pos >= buff_max_size_)
    {
        return -1;
    }
    buff_[pos] = val;
    return 0;
}

int buff_t::free_buff()
{
    if (buff_ != NULL)
    {
        free(buff_);
        buff_ = NULL;
    }
    buff_max_size_ = 0;
    data_size_t_ = 0;
    return 0;
}
int buff_t::ensure_size(uint32_t new_size)
{
    return ensure_size(new_size, true);
}

int buff_t::ensure_size(uint32_t new_size, bool ignore_copy)
{
    if (new_size <= 0)
    {
        return -1;
    }
    if (new_size <= buff_max_size_)
    {
        return 0;
    }

    unsigned char* buff1_ = (unsigned char*)malloc(new_size);
    if (buff1_ == NULL)
    {
        return -1;
    }

    if (data_size_t_ > 0 && !ignore_copy)
    {
        memcpy(buff1_, buff_, data_size_t_);
    }

    free_buff();
    buff_ = buff1_;
    buff_max_size_ = new_size;
    return 0;
}

int buff_t::copy_from(buff_t* other, uint32_t offset)
{
    return copy_mem(other->get_buff(), other->get_data_len(), offset);
}

int buff_t::copy_mem(unsigned char* mem, uint32_t length, uint32_t offset)
{
    uint32_t total = length + offset;
    int ret = ensure_size(total);
    if (ret < 0)
    {
        return ret;
    }
    memcpy(&buff_[offset], mem, length);
    data_size_t_ = total;
    return 0;
}

int buff_t::copy_to(buff_t* to_)
{
    if (to_ == NULL)
    {
        return -1;
    }
    return to_->copy_from(this);
}

int buff_t::get_buff_size() { return buff_max_size_; }
unsigned char* buff_t::get_buff() { return buff_; }
int buff_t::get_data_len() { return data_size_t_; }



void to_upper(std::string& input)
{
    std::transform(input.begin(), input.end(), input.begin(), ::toupper);

}
int local_stricmp(const char* a, const char* b  )
{
    int ca, cb;
    
    do
    {
        ca = (unsigned char)*a++;
        cb = (unsigned char)*b++;
        //ca = tolower(toupper(ca));
        //cb = tolower(toupper(cb));

        ca = toupper(ca);
        cb = toupper(cb);


    } while (ca == cb && ca != '\0');
    return ca - cb;
}