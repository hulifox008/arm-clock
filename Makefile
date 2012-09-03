font.bin: gen_font_bin
	./gen_font_bin > font.bin

gen_font_bin: gen_font_bin.c font.c
	${CC} gen_font_bin.c  -o gen_font_bin
