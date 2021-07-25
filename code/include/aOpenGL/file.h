#pragma once
#include <sys/stat.h>
#include <string>

namespace a::gl {

inline bool file_check(const std::string& name)
{
    struct stat buffer;
    return (stat (name.c_str(), &buffer) == 0); 
}

}