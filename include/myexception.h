#ifndef __MYEXCEPTION_H__
#define __MYEXCEPTION_H__

#include <string>
#include <exception>
#include <sstream>
#include <iostream>

class Exception : public std::runtime_error
{

  public:
    explicit Exception(const std::string& msg, const char * file, int line) noexcept
      : std::runtime_error(build(msg.c_str(), file, line))
    {}

    explicit Exception(const char * msg, const char * file, int line) noexcept
      : std::runtime_error(build(msg, file, line))
    {}

    virtual ~Exception()
    {}

    std::string build(const char* msg, const char* file, int line) noexcept
    {
      std::ostringstream os;

      os << file << "(" << line << "): " << msg;
      return os.str();
    }
};

#define throwException(str) { throw Exception(str, __FILE__, __LINE__); }

#endif
