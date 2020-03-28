#include "config.h"
#include <unistd.h>
#include <getopt.h>
#include "aixlog.hpp"
#include <exception>

config::config()
{}

config::~config()
{}


bool config::parse(int argc, char**argv)
{
  static struct option long_options[] = {
    { "port", required_argument, 0, 'p' },
    { 0, 0, 0, 0}
  };

  int index=0,c;
  bool err=false;

  // the '+' in the opt string sets POSIXLY_CORRECT, which means options
  // parsing will stop with the first non-option
  while((c = getopt_long(argc, argv, "+p:", long_options, &index)) != -1)
  {
    switch (c)
    {
      case 'p':
        LOG(TRACE) << "Parsed cmdline port [" << optarg << "]" << std::endl;
        try
        {
          mPort=std::stoi(optarg);
        }
        catch(std::exception& ex)
        {
          LOG(ERROR) << "Bad parameter for port [" << optarg << "] " << ex.what() << std::endl;
          err=true;
        }
        catch(...)
        {
          LOG(ERROR) << "Unknown error specifying port [" << optarg << "]" << std::endl;
          err=true;
        }
        break;
      case '?':
        // an error happened, but getopt_long will print an error msg for us
        err=true;
        break;
      default:
        // unknown error happened
        LOG(ERROR) << "getopt_long returned [" << (char)c <<"][" << c << "]" << std::endl;
        err=true;
        break;
    }
  }

  return !err;
}

void config::dump(std::ostream& os)
{
  os << "Port: " << mPort << std::endl;
}