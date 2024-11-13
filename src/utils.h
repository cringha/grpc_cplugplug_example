#pragma once
#ifndef __util_h__
#define __util_h__
#include <assert.h>
#include <mutex>
#include <thread>
#include <string>
uint64_t get_sys_time_ms();
uint64_t get_sys_tick();

void init_logs( const  std::string& logpath, const char* logname,  const char * name,  bool console);
void set_log_level(const std::string& level);

bool mkpath_if_not_exist(const std::string& logpath);
std::string path_join(const std::string& p1, const std::string p2);
bool file_exist(std::string& logpath);
std::string get_current_path();
std::string ts_to_string(uint64_t ts);


void to_upper(std::string& input);
int local_stricmp(const char* a, const char* b);

bool is_continue_loop();
void local_sleep(uint32_t ms);
void init_loop_signal();
void loop_until();
void stop_loop();

class status_t
{
private:
    std::mutex mtx_;

    //stopped , true: stopped , false:running
    bool  status_;
public:
    status_t();
    virtual ~status_t();

    bool  is_stopped();
    void  stop();
    void loop();
};


#ifdef _WIN32
std::string get_error_message(unsigned int error_code);
#endif

#ifdef USE_EVENT_TS

class event_t
{
private:
#ifdef _WIN32
    HANDLE ev_;
#endif
public:
    event_t();
    virtual ~event_t();


    int  create();
    int close();
    int  wait(uint32_t ms);
};

#endif


class buff_t
{
private:
    unsigned char* buff_;
    uint32_t        buff_max_size_; // in bytes 
    uint32_t        data_size_t_;  // in bytes 
public:
    buff_t();
    virtual ~buff_t();
    bool  is_empty();
    void empty_buff(); ;
    uint32_t set_data_size(uint32_t size);
    int  set_byte(uint32_t pos, unsigned char val); ;
    int  free_buff();
    int  ensure_size(uint32_t new_size);
    int  ensure_size(uint32_t new_size, bool  ignore_copy);
    int  copy_mem(  unsigned char* mem, uint32_t length , uint32_t offset = 0 );
    int  copy_from(buff_t* other, uint32_t offset=0);
    int  copy_to(buff_t* to_);
    int  get_buff_size();
    unsigned char* get_buff();
    int  get_data_len();

};

template<class T>
class array_t
{
private:
    buff_t buff_;
    T* header_;
    uint32_t    max_size_;
    uint32_t    end_;
public:
    array_t() {}
    virtual ~array_t() {}

    void free_buff()
    {
        buff_.free_buff();
    }
    int  ensure_size(uint32_t new_element_size)
    {
        new_element_size = new_element_size << 1;
        uint32_t len = sizeof(T) * new_element_size;
        int ret = buff_.ensure_size(len);
        if (ret)
        {
            return ret;
        }
        max_size_ = new_element_size;
        header_ = (T*)buff_.get_buff();
        return ret;
    }
    int  set(uint32_t pos, T val)
    {
        if (pos >= max_size_)
        {
            return -1;
        }

        header_[pos] = val;
        return 0;
    }

    int append(T val)
    {
        if (!ensure_size(end_ + 1))
        {
            return -1;
        }
        header_[end_] = val;
        end_ += 1;
        buff_.set_data_size( end_ * sizeof(T));
        return 0;
    }

    unsigned get_element_size()
    {
        return end_;
    }

    void* get_buff()
    {
        return buff_.get_buff();
    }
};


void  hex_dump(unsigned char* buffer,
    int length);

void init_rand();
float rand_float_r(float min_num, float max_num);
int rand_int_r(int min_num, int max_num);

int rand_int();
#endif
