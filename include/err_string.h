#ifndef __ERR_STRING_H__
#define __ERR_STRING_H__

#include <string.h>
#include <errno.h>
#include <string>
#include <vector>
#include <iostream>
#include "myexception.h"

// I want the posix strerror_r, not the gnu extension, so this is how we make
// that happen, but, only in this file
#ifdef __cplusplus
extern "C"
{
#endif
  extern int __xpg_strerror_r(int errnode, char* buffer, size_t length);
  #define strerror_r __xpg_strerror_r
#ifdef __cplusplus
}
#endif

class err_string
{
  public:

    // specific constructors
    err_string()
      : mErrno(errno)
    {
      init();
    }

    err_string(int num)
      : mErrno(num)
    {
      init();
    }

    // default copy constructor
    err_string(const err_string&) = default;

    // default assignment operator
    err_string& operator=(const err_string&) = default;

    // default destructor
    ~err_string() = default;

    // getters  getErrMsg with no param returns a const ref for efficiency
    //          getErrMsg with a param can't return a ref, so it returns by copy
    const std::string& getErrMsg()const {return mErrMsg;}
    const std::string getErrMsg(const std::string prefix)const {return prefix + mErrMsg;}
    int getErrno()const {return mErrno;}

  private:

    void init()
    {
      std::vector<char> buff;

      buff.resize(256);

      if (strerror_r(mErrno, buff.data(), buff.size()) != 0)
      {
        throwException("Error getting strerror");
      }
      mErrMsg = buff.data();
    }

    int mErrno;
    std::string mErrMsg;

};

std::ostream& operator<<(std::ostream& os, const err_string& err)
{
  os << "Error [" << err.getErrno() << "]: " << err.getErrMsg();
  return os;
}

#ifdef strerror_r
  #undef strerror_r
#endif
#endif
