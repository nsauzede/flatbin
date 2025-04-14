/* Linker script for command executable files */
OUTPUT_FORMAT(binary)
OUTPUT(a.com)
STARTUP(startup.o)
SECTIONS
{
  .main 0x10000 : { *(.main) }
  .text : { *(.text) *(.rodata) }
  .data : { *(.data) *(.bss) }
  /DISCARD/ : { *(.eh_frame) }
}
