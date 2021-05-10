#ifndef __SERVER_H__
#define __SERVER_H__

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <string.h>
#include <string>
#include <netdb.h>

class server
{
  public:
    class connection
    {
      public:
        connection(int socket, struct sockaddr_in sa_client)
          : mSocket(socket), mClient(sa_client)
        {}

        connection(const connection&) = default;
        connection& operator=(const connection&) = default;
        ~connection() = default;

        int mSocket;
        struct sockaddr_in mClient;

    };

    server();
    server(int16_t port);
    virtual ~server() = default;

    void run();

    server& setPort(uint16_t port) { mPort=port; return *this; }
    uint16_t getPort()const { return mPort; }

    server& setRoot(const std::string& root) {mRoot=root; return *this; }
    const std::string& getRoot()const {return mRoot;}

  protected:
    void service(const connection& conn);
    int create();

  private:
    // I prefer the sized int types (uint32_t) over "int", "long" etc... but when
    // you are dealing with glibc, it's best to use what it was originally written
    // with.  Sockets are old and are "int"s
    int mSocket;
    int16_t mPort;
    std::string mRoot;
};

#endif
