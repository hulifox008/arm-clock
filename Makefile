TARGET_PREFIX=arm-eabi-
TARGET_AS=${TARGET_PREFIX}as
TARGET_GCC=${TARGET_PREFIX}gcc
TARGET_LD=${TARGET_PREFIX}ld
TARGET_OBJCOPY=${TARGET_PREFIX}objcopy

clock.bin: clock.elf
	${TARGET_OBJCOPY} -j ".text" -O binary clock.elf clock.bin

font.bin: gen_font_bin
	./gen_font_bin > font.bin

font.o: font.S font.bin
	arm-eabi-as font.S -o font.o


gen_font_bin: gen_font_bin.c font.c
	${CC} gen_font_bin.c  -o gen_font_bin

clock.elf: init.o clock.o font.o
	${TARGET_LD} -g -T ld.ld $^ -o clock.elf

init.o: init.S
	${TARGET_AS} -k -mthumb -mlittle-endian init.S -o init.o

clock.o: clock.c
	${TARGET_GCC}  -O2 -mthumb -mcpu=cortex-m3 -c clock.c -o clock.o

clean:
	rm -fr gen_font_bin
	rm -fr font.bin
	rm -fr *.o
	rm -fr clock.bin
	rm -fr clock.elf
