#include "lpc17xx.h"

void gpio_init()
{
    /* P0.0-P0.7   output */
    /* P0.16-P0.23 output */
    /* P1.0-P1.11  output */
    writeb(0, PINSEL0);
    writeb(0, PINSEL0+1);
    writeb(0, PINSEL1);
    writeb(0, PINSEL1+1);
    writeb(0, PINSEL2);
    writeb(0, PINSEL2+1);
    writeb(0, PINSEL2+2);

    /* All used GPIO pins have pull-up */
    writeb(0, PINMODE0);
    writeb(0, PINMODE0+1);
    writeb(0, PINMODE1);
    writeb(0, PINMODE1+1);
    writeb(0, PINMODE2);
    writeb(0, PINMODE2+1);
    writeb(0, PINMODE2+2);

    /* All used GPIO pins are output */
    writeb(0, FIO0DIR);
    writeb(0, FIO0DIR+2);
    writeb(0, FIO1DIR);
    writeb(0, FIO1DIR+1);

    writeb(0, FIO0MASK);
    writeb(0, FIO0MASK+2);

    /* P1.0 P1.1 P1.4 */
    writeb(0xEC, FIO1MASK);

    /* P1.8 P1.9 */
    writeb(0xFC, FIO1MASK+1);


    /* set GPIO initial status 
     CS_n -> H
     RS   -> H
     RD_n -> H
     WR_n -> L
     RESET_n -> L
     */
    writeb(0xFF, FIO1PIN);
    writeb(0xFC, FIO1PIN+1);
}

int clock_main()
{
    gpio_init();
    return 0;
}

