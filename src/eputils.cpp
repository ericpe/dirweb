#include "eputils.h"

void hexdump
(const unsigned char* pBuff,
 const unsigned int Len,
 std::ostream& os,
 bool Offset,
 bool Printable)
{
  char buff[64];
  unsigned int i,j,k;

  for(i=0;i<Len;i+=16)
  {
    if(Offset)
    {
      sprintf(buff,"%04X: ",i);
      os<<buff;
    }

    for(j=0;j<16;j++)
    {
      k=j+i;
      if(k<Len)
      {
        sprintf(buff, "%02X ",pBuff[k]);
        os<<buff;
      }
      else
        os<<"   ";
    }

    if(Printable)
    {
      os<<" |";
      for(j=0;j<16;j++)
      {
        k=j+i;
        if(k<Len)
        {
          if(isprint(pBuff[k]) && pBuff[k]>=' ')
          {
            sprintf(buff,"%c",pBuff[k]);
            os<<buff;
          }
          else
            os<<".";
        }
        else
          os<<" ";
      }
      os<<"|"<<std::endl;
    }
  }
}

std::string asHex
(unsigned int val,
 unsigned char Minwidth,
 bool Zerofill,
 bool Uppercase,
 bool Showbase)
{
  std::ostringstream os;

  if(Showbase)
    os<<"0x";
  if(Minwidth>0)
    os<<std::setw(Minwidth);
  if(Zerofill)
    os<<std::setfill('0');
  if(Uppercase)
    os<<std::uppercase;

  os<<std::hex<<val;

  return os.str();
}

