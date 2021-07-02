ENTRY(_start)
OUTPUT_FORMAT(binary)

SECTIONS
{
    . = 0x100000;
    .text :
    {
        *(.text)
    }

    .rodata :
    {
        *(.rodata)
    }

    .data :
    {
        *(.data)
    }

    .bss :
    {
        *(COMMON)
        *(.bss)
    }

    .asm :
    {
        *(.asm)
    }
}