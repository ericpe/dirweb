#ifndef __EPUTILS_H__
#define __EPUTILS_H__

#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>


void hexdump (const unsigned char* pBuff,
              const unsigned int Len,
              std::ostream& os=std::cout,
              bool Offset=true,
              bool Printable=true);

std::string asHex(unsigned int val,
                  unsigned char Minwidth=2,
                  bool Zerofill=true,
                  bool Uppercase=true,
                  bool Showbase=true);

#endif
