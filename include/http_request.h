#ifndef __HTTP_REQUEST_H__
#define __HTTP_REQUEST_H__

#include <vector>
#include "err_string.h"
#include "eputils.h"

class http_request
{
  public:
    static const int block_size;
    typedef std::vector<uint8_t> block;
    typedef std::vector<block> blocklist;

    int getDataLenWaiting(int);
    bool read(int socket);

  private:
    blocklist mBlockList;
};

#endif
