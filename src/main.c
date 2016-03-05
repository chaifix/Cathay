#include "cathay.h"
#include <stdlib.h>

int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        printf("Usage:\n    cathay <portnumber>\n");
        return 1; 
    }

    int portnumber = atoi(argv[1]);

    ct_Server* server = ct_newserver(portnumber);

    if (server == 0)
    {
        printf("Launch cathay failed.\n");

        return 1;
    }

    printf("cathay started\n");
    ct_startserver(server);
    printf("cathay stopped\n");
    ct_stopserver(server);
    printf("cathay closed\n");
    ct_closeserver(server);
}
