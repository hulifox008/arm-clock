SECTIONS
{
    . = 0x10000000;  /* Image is loaded to RAM at 0x10000000 */
    .text : {*(.init) *(.text) *(.data) }
    .bss  SIZEOF(.text): 
    {*(.bss)}
}
