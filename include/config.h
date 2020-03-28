#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <stdint.h>
#include <iostream>
#include <string>


class config
{
  public:
    // since we can't initialize ALL of the consts here (const char * isn't
    // an integral type), I've elected to initialize them all in the cpp file.
    static const uint16_t default_port;
    static const char * default_root;

  public:
    config();
    virtual ~config();

    bool parse(int argc, char**argv);

    uint16_t getPort()const {return mPort;}

    void dump(std::ostream& os=std::cout);

    std::string tostring();

  private:
    uint32_t mPort;

};

#endif


