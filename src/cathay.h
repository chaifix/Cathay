#ifndef _CATHAY_H
#define _CATHAY_H

#include "libs/tekcos/tekcos.h"

#define CATHAT_VERSION "0.1.0"

typedef struct
{
    tk_TCPsocket* sk;
}ct_Server;

ct_Server* ct_newserver(uint16 portnumber); 

int ct_startserver(ct_Server* server);

int ct_stopserver(ct_Server* server);

int ct_closeserver(ct_Server* server);

#endif