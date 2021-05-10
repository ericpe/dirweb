#include "http_request.h"
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "aixlog.hpp"
#include "myexception.h"

const int http_request::block_size = 4096;

int http_request::getDataLenWaiting(int socket)
{
  int count = 0;

  if (ioctl(socket, FIONREAD, &count) != 0)
  {
    err_string err;
    throwException(err.getErrMsg("ioctl: "));
  }

  LOG(TRACE) << "amount of data waiting [" << count << "]" << std::endl;
  return count;
}

bool http_request::read(int socket)
{
  ssize_t sz;

  //while(getDataLenWaiting(socket) > 0 )
  while(true )
  {
    // there is data waiting
    // 1. add a new block to the blocklist (emplace)
    // 2. size the block correctly
    // 2. use the block without copyinig

    mBlockList.emplace_back(block_size);
    uint8_t *pBlk = mBlockList.back().data();

    if ((sz = ::read(socket, pBlk, block_size)) < 0 )
    {
      err_string err;
      throwException(err.getErrMsg("read: "));
    }
    LOG(TRACE) << "request read " << sz << " bytes" << std::endl;
    break;
  }

  if (WOULDLOG(DEBUG))
  {
    int32_t num = 0;
    for(const auto& buf : mBlockList)
    {
      num++;
      std::ostringstream os;

      hexdump(buf.data(), block_size, os);

      LOG(DEBUG) << "Buffer " << num << ":" << std::endl << os.str() << std::endl;
    }
  }

  return false;
}
