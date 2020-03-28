#ifndef __SERVER_H__
#define __SERVER_H__

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <string.h>
#include <errno.h>
#include <string>
#include <netdb.h>

class server
{
  public:

    const int16_t default_port = 3127; // arbitrary number

    server();
    server(int16_t port);
    virtual ~server();

    void run();

    void setPort(uint16_t port) { mPort=port; }
    uint16_t getPort()const { return mPort; }

  protected:
    int create();

  private:
    // I prefer the sized int types (uint32_t) over "int", "long" etc... but when
    // you are dealing with glibc, it's best to use what it was originally written
    // with.  Sockets are old and are "int"s
    int mSocket;
    int16_t mPort;
};

#endif
