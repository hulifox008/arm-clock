#include "lpc17xx.h"

/* DS1623 connection
 * p1.18    DQ
 * p1.19    CLK_n
 * p1.20    RST_n
 */

extern unsigned char fontdata;
extern int fontdata_size;

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
    writeb(0, PINSEL3);
    writeb(0, PINSEL3+1);

    /* All used GPIO pins have pull-up */
    writeb(0, PINMODE0);
    writeb(0, PINMODE0+1);
    writeb(0, PINMODE1);
    writeb(0, PINMODE1+1);
    writeb(0, PINMODE2);
    writeb(0, PINMODE2+1);
    writeb(0, PINMODE2+2);
    writeb(0, PINMODE3);
    writeb(0, PINMODE3+1);

    /* All used GPIO pins are output */
    writeb(0xFF, FIO0DIR);
    writeb(0xFF, FIO0DIR+2);
    writeb(0xFF, FIO1DIR);
    writeb(0xFF, FIO1DIR+1);
    writeb(0xFF, FIO1DIR+2);

    writeb(0, FIO0MASK);
    writeb(0, FIO0MASK+2);

    /* P1.0 P1.1 P1.4 */
    writeb(0xEC, FIO1MASK);

    /* P1.8 P1.9 */
    writeb(0xFC, FIO1MASK+1);

    /* p1.18, p1.19, p1.20 are used to access DS1623 */
    writeb(0xE3, FIO1MASK+2);


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

void pll0_feed()
{
    writeb(0xAA, PLL0FEED);
    writeb(0x55, PLL0FEED);
}

/* Initialize the PLL0. By using 12M oscillator, this set N to 1 and M to 12. PLL0 output frequency is 288MHz. CPU devider is set to 4 to get a 72M CPU clock. */
void pll0_init()
{
    /* disconnect pll0 */
    writeb(readb(PLL0CON) & ~0x02, PLL0CON);
    pll0_feed();
    
    /* disable ppl0 */
    writeb(readb(PLL0CON) & ~0x01, PLL0CON);
    pll0_feed();

    /* N=1 M=12 */
    writel(0x0000000B, PLL0CFG);
    pll0_feed();

    /* Enable pll0. */
    writeb(0x01, PLL0CON);
    pll0_feed();

    /* set CPU clock devider to 4. */
    writeb(0x04, CCLKSEL);

    /* wait the pll to lock. */
    while(!(readl(PLL0STAT)&0x04000000));

    /* connect pll0 */
    writeb(0x03, PLL0CON);
    pll0_feed();
}

/* timer0 is dedicated used by ms_delay() */
void timer0_init()
{
    /* timer0 running at 1/2 cpu clock. */
    #define TIMER0_PCLK  (72000000/2)

    /* TIMER0 clock = 1/2 CPU clock */
    writeb(0x08, PCLKSEL0);

    /* reset the timer. */
    writeb(0x02, T0TCR);

    /* timer0 timer mode. */
    writeb(0, T0CTCR);

    writel(0, T0TC);
    writel(0, T0PR);
    writel(TIMER0_PCLK/1200, T0MR0); /* 1ms timer. */
    writel(0x04, T0MCR);
    writel(0, T0CCR);
}

void sys_init()
{
    /* Disable all interrupts */
    writel(0xFFFFFFFF, ICER0);
    writel(0xFFFFFFFF, ICER1);

    /* select main oscillator as PLL0 input. */
    writeb(0x01, CLKSRCSEL);

    pll0_init();

    /* disable MAT0.0 on P1.28 */
    writeb(readb(PINSEL3+3) & ~0x03, PINSEL3+3);

    /* enable RTC */
    writeb(1, RTC_CCR);

}

void ms_delay(unsigned int ms)
{
    if(ms==0)
         return ;

    /* reset the timer. */
    writeb(0x02, T0TCR);

    writel(0, T0TC);
    writel(ms-1, T0PR);

    /* FIXME: should do bit access here. */
    writeb(0x11, T0EMR);
    /* start the timer. */
    writeb(0x01, T0TCR);

    while(readb(T0EMR) & 0x01);
}

inline void hy32b_set_reg(unsigned char reg)
{
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

    /* RS high, using bit-bang address. */
    writeb(1, P1_BITBANG+4);
    dmb();

}

inline void hy32b_write(unsigned short val)
{
    writeb(val&0xFF, FIO0PIN);
    writeb((val>>8)&0xFF, FIO0PIN+2);

    /* Strobe CS_n, using bit-bang address. */
    writeb(0, P1_BITBANG);
    dmb();
    writeb(1, P1_BITBANG);
    dmb();
}

void hy32b_write_reg(unsigned char reg, unsigned short val)
{
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

    /* RS high, using bit-bang address. */
    writeb(1, P1_BITBANG+4);
    dmb();

    /* Strobe CS_n, using bit-bang address. */
    writeb(0, P1_BITBANG);
    dmb();
    writeb(1, P1_BITBANG);
    dmb();

}

void lcd_init()
{
    /* reset LCD, reset pin is connected to p1.9 */
    writeb(0, P1_BITBANG+9*4); 
    ms_delay(50);
    writeb(1, P1_BITBANG+9*4); 
    ms_delay(50);

    hy32b_write_reg(0x0000, 0x0001);
    hy32b_write_reg(0x0007, 0x0023);
    hy32b_write_reg(0x0010, 0x0000);

    ms_delay(50);

    hy32b_write_reg(0x0007, 0x0033);
    hy32b_write_reg(0x0011, 0x6070);
    hy32b_write_reg(0x0002, 0x0600);
    hy32b_write_reg(0x0003, 0x0804);
    hy32b_write_reg(0x000C, 0x0000);
    hy32b_write_reg(0x000D, 0x0808);
    hy32b_write_reg(0x000E, 0x2900);
    hy32b_write_reg(0x001E, 0x00B8);
    hy32b_write_reg(0x0001, 0x2B3F);
    hy32b_write_reg(0x0010, 0x0000);
    hy32b_write_reg(0x0005, 0x0000);
    hy32b_write_reg(0x0006, 0x0000);
    hy32b_write_reg(0x0016, 0xEF1C);
    hy32b_write_reg(0x0017, 0x0003);
    hy32b_write_reg(0x0007, 0x0233);
    hy32b_write_reg(0x000B, 0x0000);
    hy32b_write_reg(0x000F, 0x0000);
    hy32b_write_reg(0x0041, 0x0000);
    hy32b_write_reg(0x0042, 0x0000);
    hy32b_write_reg(0x0048, 0x0000);
    hy32b_write_reg(0x0049, 0x013F);
    hy32b_write_reg(0x004A, 0x0000);
    hy32b_write_reg(0x004B, 0x0000);
    hy32b_write_reg(0x0044, 0xEF00); /* 240 */
    hy32b_write_reg(0x0045, 0x0000); /* starts at 0 */
    hy32b_write_reg(0x0046, 0x013F);/* ends at 319 */
    hy32b_write_reg(0x0030, 0x0707);
    hy32b_write_reg(0x0031, 0x0204);
    hy32b_write_reg(0x0032, 0x0204);
    hy32b_write_reg(0x0033, 0x0502);
    hy32b_write_reg(0x0034, 0x0507);
    hy32b_write_reg(0x0035, 0x0204);
    hy32b_write_reg(0x0036, 0x0204);
    hy32b_write_reg(0x0037, 0x0502);
    hy32b_write_reg(0x003A, 0x0302);
    hy32b_write_reg(0x003B, 0x0302);
    hy32b_write_reg(0x0023, 0x0000);
    hy32b_write_reg(0x0024, 0x0000);
    hy32b_write_reg(0x0025, 0x8000);
    hy32b_write_reg(0x004f, 0);
    hy32b_write_reg(0x004e, 0);

    hy32b_write_reg(0x0022, 0);

}

void lcd_fill_rect(unsigned char x, unsigned short y, unsigned char w, unsigned short h, unsigned short color)
{
    hy32b_write_reg(0x0044, (x+w-1)<<8 | x);
    hy32b_write_reg(0x0045, y);
    hy32b_write_reg(0x0046, y+h-1);

    hy32b_write_reg(0x004e, x);
    hy32b_write_reg(0x004f, y);

    hy32b_set_reg(0x0022);
    unsigned int i;
    for(i=0;i<w*h;i++)
            hy32b_write(color);
}

void lcd_show_tile(unsigned char x, unsigned short y, unsigned char t, unsigned short color)
{
    unsigned char * p = (char*)&fontdata+(t*280); /* each tile is 280 bytes. */

    hy32b_write_reg(0x0044, (x+39)<<8 | x);  /* each tile is 40 pixel in width */
    hy32b_write_reg(0x0045, y);
    hy32b_write_reg(0x0046, y+55); /* each tile is 56 pixel in height. */

    hy32b_write_reg(0x004e, x);
    hy32b_write_reg(0x004f, y);

    hy32b_set_reg(0x0022);
    unsigned int i = 0;
    for(i=0;i<280;i++)
    {
        unsigned char c = *(p+i);
        unsigned int b = 0;
        for(b=0;b<8;b++)
        {
            if(c&0x80)
                hy32b_write(color);
            else
                hy32b_write(0x0000);
            c=c<<1;
        }
    }
}

void update_clock()
{
    unsigned int  t = readl(0x40024014);
    unsigned char s = readb(0x40024020);
    unsigned char m = readb(0x40024024);

    lcd_show_tile(20, 50, m/10, 0x0FF0);
    lcd_show_tile(60, 50, m%10, 0x0FF0);
    lcd_show_tile(100, 50, 10, 0x0FF0);
    lcd_show_tile(140, 50, s/10, 0x0FF0);
    lcd_show_tile(180, 50, s%10, 0x0FF0);
}

void delay()
{
    int i = 0;
    volatile int t;
    for(i=0;i<100;i++)
        t=0;
}

inline void ds1623_rst_hi()
{
    writeb(1, BITBAND_ADDR(0x22000000, FIO1PIN-0x20000000, 20));
    dmb();
}

inline void ds1623_rst_lo()
{
    writeb(0, BITBAND_ADDR(0x22000000, FIO1PIN-0x20000000, 20));
    dmb();
}

inline void ds1623_clk_hi()
{
    writeb(1, BITBAND_ADDR(0x22000000, FIO1PIN-0x20000000, 19));
    dmb();
}

inline void ds1623_clk_lo()
{
    writeb(0, BITBAND_ADDR(0x22000000, FIO1PIN-0x20000000, 19));
    dmb();
}

inline void ds1623_dq_hi()
{
    writeb(1, BITBAND_ADDR(0x22000000, FIO1PIN-0x20000000, 18));
    dmb();
}

inline void ds1623_dq_lo()
{
    writeb(0, BITBAND_ADDR(0x22000000, FIO1PIN-0x20000000, 18));
    dmb();
}

inline void ds1623_dq_input()
{
    writeb(0, BITBAND_ADDR(0x22000000, FIO1DIR-0x20000000, 18));
    dmb();
}

inline void ds1623_dq_output()
{
    writeb(1, BITBAND_ADDR(0x22000000, FIO1DIR-0x20000000, 18));
    dmb();
}

inline unsigned int ds1623_dq()
{
    return readb(BITBAND_ADDR(0x22000000, FIO1PIN-0x20000000, 18));
}

void ds1623_send_command(unsigned char cmd)
{
    int i;
    for(i=0;i<8;i++)
    {
        ds1623_clk_lo();
        if(cmd&1)
            ds1623_dq_hi();
        else
            ds1623_dq_lo();

        delay();
        ds1623_clk_hi();
        delay();
        cmd = cmd>>1;
    }
}

void ds1623_init()
{
    ds1623_rst_lo();
    ds1623_clk_hi();
    ds1623_dq_output();
    ms_delay(10);
    
    ds1623_rst_hi();
    delay();
    ds1623_send_command(0xEE);
    ds1623_rst_lo();
    delay();
}

unsigned char ds1623_read_temp()
{
    ds1623_rst_hi();
    delay();
    ds1623_send_command(0xAA);
    
    int i;
    unsigned char t = 0;
    ds1623_dq_input();
    delay();
    for(i=0;i<9;i++)
    {
        t=t>>1;
        ds1623_clk_lo();
        delay();
        if(ds1623_dq())
            t|=0x80;
        ds1623_clk_hi();
        delay();
    }
    ds1623_dq_output();
    ds1623_rst_lo();
    delay();
    return t;
}

void update_temp()
{
    unsigned char t = ds1623_read_temp();
    lcd_show_tile(50, 200, t/10, 0xF000);
    lcd_show_tile(90, 200, t%10, 0xF000);
    lcd_show_tile(150, 200, 11, 0xF000);
}
int clock_main()
{
    sys_init();
    gpio_init();
    timer0_init();
    lcd_init();

    ds1623_init();

    lcd_fill_rect(0, 0, 240, 320, 0x0000);
    int i = 0;
    for(;;)
    {
        ms_delay(1000);
        update_clock();
        update_temp();
    }
    return 0;
}

