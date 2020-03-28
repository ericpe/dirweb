#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <stdint.h>
#include <iostream>

class config
{
  public:
    config();
    virtual ~config();

    bool parse(int argc, char**argv);

    uint32_t getPort()const {return mPort;}

    void dump(std::ostream& os=std::cout);

  private:
    uint32_t mPort;

};

#endif


