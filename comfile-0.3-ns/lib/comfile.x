/* Linker script for command executable files */
OUTPUT_FORMAT(binary)
OUTPUT(a.com)
STARTUP(startup.o)
SECTIONS
{
  .main 0x10000 : { *(.main) }
  .text : { *(.text) *(.rodata) }
  .data : { *(.data) *(.bss) }
  /DISCARD/ : {
    *(.bss)
    *(.note*)
    *(.eh_frame*)
    *(.comment)
    *(.rela*)
    *(.symtab)
    *(.strtab)
    *(.shstrtab)
  }
}
