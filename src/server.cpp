#include "server.h"
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "aixlog.hpp"
#include "config.h"
#include "myexception.h"
#include "err_string.h"
#include "eputils.h"

// we want to initialize the port to something.  if we use 0, then we need to 
// put all kinds of checks in.  if we put in the default port from config, it
// will work without checks
server::server()
  : mSocket(-1),mPort(config::default_port)
{ }

server::server(int16_t port)
  : mSocket(-1),mPort(port)
{ }

void server::run()
{
  struct sockaddr_in sa_client;
  int num,sock;

  create();

  while(true)
  {
    printf("waiting on connection (port %d)\n",mPort);

    do
    {
      fd_set fdset;
      struct timeval to;
      FD_ZERO(&fdset);
      FD_SET(mSocket,&fdset);
      memset(&to,0,sizeof(to));
      to.tv_usec=250*1000;

      num=select(mSocket+1,&fdset,NULL,NULL,&to);
      if(num==-1)
      {
        // you want to create an err_string immediately after the error because
        // it's going to use errno, which will get clobbered otherwise.
        err_string err;
        LOG(ERROR) << "select failed" << err << std::endl;
      }

      socklen_t len=sizeof(sa_client);
      if((sock=accept(mSocket,(struct sockaddr*)&sa_client,&len))==-1)
      {
        err_string err;
        LOG(ERROR) << "accept failed" << err << std::endl;
      }

      LOG(INFO) << "Connection from " << inet_ntoa(sa_client.sin_addr)
                << ":" << sa_client.sin_port << std::endl;

      service(connection(sock, sa_client));
    }
    while(num==0);
  }
}

void server::service(const server::connection& conn)
{
  uint8_t buff[4096];
  ssize_t sz;

  memset(buff, 0, sizeof(buff));

  if((sz = recv(conn.mSocket, buff, sizeof(buff),0)) == -1 )
  {
    err_string err;
    throwException(err.getErrMsg("recv: "));
  }

  LOG(TRACE) << "request size " << sz << " (" << asHex(sz) << ")" << std::endl;
  hexdump(buff, (sz > 128 ? sz : 128));

  std::cout << std::endl << "---" << std::endl << buff << "---" << std::endl;

  // for now, just send back a 404

  const char* header_template = R"raw(HTTP/1.1 404 Not Found
Server: nginx/1.14.2
Date: Thu, 02 Apr 2020 23:23:04 GMT
Content-Type: text/html; charset=koi8-r
Content-Length: %d
Connection: keep-alive

)raw";

const char* data = R"raw(<html>
<head><title>404 Not Found</title></head>
<body bgcolor="white">
<center><h1>404 Not Found</h1></center>
<hr><center>nginx/1.14.2</center>
</body>
</html>
)raw";
  int dsz = strlen(data);

  std::vector<char> header;
  header.resize(strlen(header_template)+16);
  int hsz = snprintf(header.data(), header.size(), header_template, dsz);

  LOG(TRACE) << "Returning 404 hsz " << hsz << " dsz " << dsz << std::endl;

  send(conn.mSocket, header.data(), hsz, 0);
  send(conn.mSocket, data, dsz, 0);

}

int server::create()
{
  int num;
  struct sockaddr_in addr;

  memset(&addr,0,sizeof(addr));

  if((mSocket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))<0)
  {
    throwException(std::string("creating socket") + strerror(errno));
    //throw std::runtime_error(std::string("creating socket") + strerror(errno));
    //perror(strerror(errno));
  }

  setsockopt(mSocket,SOL_SOCKET,SO_REUSEADDR,&num,sizeof(num));

  memset(&addr,0,sizeof(addr));
  addr.sin_family=AF_INET;
  addr.sin_addr.s_addr=INADDR_ANY;
  addr.sin_port=htons(mPort);

  if((bind(mSocket,(struct sockaddr*)&addr,sizeof(addr)))==-1)
  {
    err_string err;
    throwException(err.getErrMsg("binding: "));
  }

  if((listen(mSocket,5))==-1)
  {
    err_string err;
    throwException(err.getErrMsg("listening: "));
  }

  return mSocket;
}
