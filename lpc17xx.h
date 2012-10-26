#ifndef _LPC17XX_INCLUDED_
#define _LPC17XX_INCLUDED_

#define FIO0DIR     0x2009C000
#define FIO1DIR     0x2009C020
#define FIO2DIR     0x2009C040
#define FIO3DIR     0x2009C060
#define FIO4DIR     0x2009C080

#define FIO0MASK    0x2009C010
#define FIO1MASK    0x2009C030
#define FIO2MASK    0x2009C050
#define FIO3MASK    0x2009C070
#define FIO4MASK    0x2009C090

#define FIO0PIN     0x2009C014
#define FIO1PIN     0x2009C034
#define FIO2PIN     0x2009C054
#define FIO3PIN     0x2009C074
#define FIO4PIN     0x2009C094

#define P1_BITBANG  0x23380680

#define RTC_CCR     0x40024008

#define PINSEL0     0x4002C000
#define PINSEL1     0x4002C004
#define PINSEL2     0x4002C008
#define PINSEL3     0x4002C00C

#define PINMODE0    0x4002C040
#define PINMODE1    0x4002C044
#define PINMODE2    0x4002C048
#define PINMODE3    0x4002C04C

#define T0IR        0x40004000
#define T0TCR       0x40004004
#define T0TC        0x40004008
#define T0PR        0x4000400C
#define T0PC        0x40004010
#define T0MCR       0x40004014
#define T0MR0       0x40004018
#define T0MR1       0x4000401C
#define T0MR2       0x40004020
#define T0MR3       0x40004024
#define T0CCR       0x40004028
#define T0CR0       0x4000402C
#define T0CR1       0x40004030
#define T0EMR       0x4000403C
#define T0CTCR      0x40004070

#define ISER0       0xE000E100
#define ISER1       0xE000E104
#define ICER0       0xE000E180
#define ICER1       0xE000E184

#define CCLKSEL     0x400FC104
#define CLKSRCSEL   0x400FC10C

#define PLL0CON     0x400FC080
#define PLL0CFG     0x400FC084
#define PLL0STAT    0x400FC088
#define PLL0FEED    0x400FC08C

#define SCS         0x400fC1A0
#define PCLKSEL0    0x400FC1A8
#define PCLKSEL1    0x400FC1AC

/* 
 * base       : bitband alias base address.
 * byte_offset: offset of byte which contain desired bit.
 * bit_offfset: bit offset in its byte (0 ~ 7)
 */
#define BITBAND_ADDR(base, byte_offset, bit_offset) \
((base)+(byte_offset)*32+(bit_offset)*4)

#define writeb(v, a)    (*(volatile unsigned char *)(a) = (v))
#define writew(v, a)    (*(volatile unsigned short *)(a) = (v))
#define writel(v, a)    (*(volatile unsigned int *)(a) = (v))

#define readb(a)        (*(volatile unsigned char*)(a))
#define readw(a)        (*(volatile unsigned short*)(a))
#define readl(a)        (*(volatile unsigned int*)(a))

#define dmb() __asm__ __volatile__ ("dmb" : : : "memory")
#define isb() __asm__ __volatile__ ("isb" : : : "memory")
#define dsb() __asm__ __volatile__ ("dsb" : : : "memory")

#endif /* _LPC17XX_INCLUDED_ */
