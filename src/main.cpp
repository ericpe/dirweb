#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <string.h>
#include <errno.h>
#include <string>
#include <netdb.h>

#include "aixlog.hpp" // from https://github.com/ericpe/aixlog

#include "server.h"
#include "config.h"
#include <exception>

int main(int argc, char **argv)
{
  AixLog::Severity severity = AixLog::Severity::trace;

  AixLog::Log::init(
    {
      // Log to the console
      std::make_shared<AixLog::SinkCout> (severity,
                                          AixLog::Type::all,
                                          "%Y-%m-%d %H:%M:%S.#ms #file(#line) [#severity]: #message"),
      // Log everything to logfile.log
      std::make_shared<AixLog::SinkFile> (severity,
                                          AixLog::Type::all,
                                          "logfile.log",
                                          "%Y-%m-%d %H:%M:%S.#ms #file(#line) [#severity]: #message")
    }
  );

  LOG(INFO) << "Logging Started" << std::endl;


  config cfg;

  if(!cfg.parse(argc, argv))
  {
    LOG(ERROR) << "commandline parsing error" << std::endl;
    return 1;
  }

  LOG(INFO) << cfg.tostring();

  server srv;

  srv.setPort(cfg.getPort()).setRoot(cfg.getRoot());

  LOG(INFO) << "Starting server" << std::endl;

  try
  {
    srv.run();
  }
  catch(std::exception& ex)
  {
    LOG(ERROR) << "Caught an exception!!  [" << ex.what() << "]" << std::endl;
  }


  printf("hello\n");
  LOG(INFO) << "Stopping" << std::endl;
  return 0;
}
