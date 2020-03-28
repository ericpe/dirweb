#include "server.h"
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "aixlog.hpp"
#include "config.h"
#include "myexception.h"

server::server()
  : mSocket(-1),mPort(server::default_port)
{ }

server::server(int16_t port)
  : mSocket(-1),mPort(port)
{ }

server::~server()
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
        LOG(ERROR) << "select failed" << std::endl;
        perror("select failed");
      }

      socklen_t len=sizeof(sa_client);
      if((sock=accept(mSocket,(struct sockaddr*)&sa_client,&len))==-1)
        perror(strerror(errno));

      LOG(INFO) << "Connection from " << inet_ntoa(sa_client.sin_addr)
                << ":" << sa_client.sin_port << std::endl;
    }
    while(num==0);
  }
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
    throwException(std::string("binding: ") + strerror(errno));
    //throw std::runtime_error(std::string("binding: ") + strerror(errno));
    //perror(strerror(errno));
  }

  if((listen(mSocket,5))==-1)
  {
    throw std::runtime_error(std::string("listening: ") + strerror(errno));
    perror(strerror(errno));
  }

  return mSocket;
}
