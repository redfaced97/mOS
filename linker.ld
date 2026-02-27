OUTPUT_FORMAT(elf32-i386)
ENTRY(start) /* Убедитесь, что в entry.s метка называется start, а не _start */

PHDRS
{
  text PT_LOAD FLAGS(5); /* Read + Execute */
  data PT_LOAD FLAGS(6); /* Read + Write */
}

SECTIONS
{
  /* Ядро загружается по адресу 1 MiB */
  . = 0x00100000;

  .text : {
    /* Multiboot header должен быть в первых 8 KiB файла */
    KEEP(*(.multiboot))
    *(.text)
    *(.text*)
    *(.rodata)
    *(.rodata*)
  } :text

  .data : {
    *(.data)
    *(.data*)
  } :data

  .bss : {
    _bss_start = .;
    *(.bss)
    *(.bss*)
    *(COMMON)
    _bss_end = .;
  } :data

  /* Обозначаем границы свободной памяти, НЕ раздувая файл */
  . = ALIGN(4096);
  _heap_start = .;
  
  /* Символические метки для вашего менеджера памяти */
  _kernel_end = .;
}
