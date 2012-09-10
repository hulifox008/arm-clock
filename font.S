.global fontdata
.global fontdata_size
.section .data

fontdata:
    .incbin "font.bin"
1:
fontdata_size:
    .int 1b - fontdata
