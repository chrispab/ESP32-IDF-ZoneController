#ifndef __FAN_H
#define __FAN_H

#include "config.h"
#include "IOBase.h"

class Fan: public IOBase
{
  protected:

  public:
    Fan();
    void control(long millis);
};

#endif
