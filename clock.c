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

void sys_init()
{
    /* Disable all interrupts */
    writel(0xFFFFFFFF, ICER0);
    writel(0xFFFFFFFF, ICER1);

    /* P1.28 selected as MAT0.0 */
    writeb(readb(PINSEL3+3)|0x03, PINSEL3+3);

}

void ms_delay()
{
    /* timer0 timer mode. */
    writeb(0, T0CTCR);

    writel(0, T0TC);
    writel(0, T0PR);
    writel(1000, T0MR0);
    writel(0x02, T0MCR);
    writel(0, T0CCR);

    /* Toggle external match MAT0.0 */
    writew(0x0030, T0EMR);
}

void hy32b_write_reg(unsigned char reg, unsigned short val)
{
    #define P1_BITBANG  0x23380680

    /* put reg address on data bus */
    writeb(reg, FIO0PIN);
    writeb(0, FIO0PIN+2);

    /* RS low, using bit-bang address. */
    writeb(0, P1_BITBANG+4);
    dmb();

    /* Strobe CS_n, using bit-bang address. */
    writeb(0, P1_BITBANG);
    dmb();
    writeb(1, P1_BITBANG);
    dmb();

    writeb(val&0xFF, FIO0PIN);
    writeb((val>>8)&0xFF, FIO0PIN+2);

    /* RS low, using bit-bang address. */
    writeb(1, P1_BITBANG+4);
    dmb();

    /* Strobe CS_n, using bit-bang address. */
    writeb(0, P1_BITBANG);
    dmb();
    writeb(1, P1_BITBANG);
    dmb();

}

int clock_main()
{
    sys_init();
//    gpio_init();
    ms_delay();
    for(;;);
    return 0;
}

