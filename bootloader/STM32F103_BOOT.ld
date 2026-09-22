ENTRY(Reset_Handler)

MEMORY
{
  FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 12K
  RAM (xrw)  : ORIGIN = 0x20000000, LENGTH = 20K
}

_estack = ORIGIN(RAM) + LENGTH(RAM);

SECTIONS
{
  .isr_vector : { KEEP(*(.isr_vector)) } > FLASH
  .text : { *(.text*) *(.rodata*) *(.ARM.exidx*) _etext = .; } > FLASH
  .data : AT(_etext) { _sdata = .; *(.data*) _edata = .; } > RAM
  _sidata = LOADADDR(.data);
  .bss (NOLOAD) : { _sbss = .; *(.bss*) *(COMMON) _ebss = .; } > RAM
  end = .;
}
