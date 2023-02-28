#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include <xevict_normal_write.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define ARR_SIZE 4096
#define WRITES 2
#define STRIDE 32 // cache line size
#define MODE 0
#define WAE 0
#define WBE 0
#define INTERLEAVES 0


int main()
{
    init_platform();

    XEvict_normal_write evicter;
    unsigned char array[4096];
    unsigned int config[1024];

    // bringing array in cache
    for(int i=0; i<ARR_SIZE; i++){
    	array[i] = 0;
    }

    xil_printf("\e[1;1H\e[2J");
    xil_printf("Starting \"normal write evicter\"\n\r");
    XEvict_normal_write_Initialize(&evicter, 0);
    XEvict_normal_write_Set_array_r(&evicter, (UINTPTR) array);
    XEvict_normal_write_Set_config_r(&evicter, config);

    // setting the params
    config[0] = WRITES;
    config[1] = STRIDE;
    config[2] = MODE;
    config[3] = WAE;
    config[4] = WBE;
    config[5] = INTERLEAVES;

    XEvict_normal_write_Start(&evicter);
    while(!XEvict_normal_write_IsDone(&evicter));

    xil_printf("Program terminated\n\r");

    cleanup_platform();
    return 0;
}
