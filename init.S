.section .init

start:
    ldr     r0, =0x10008000
    mov     sp, r0
    bl      clock_main

loop:
    b      loop
