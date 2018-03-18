SECTIONS
{
  .rodata :
  {
    __LIBS_LIST__ = .;
    QUAD((__LIBS_END__ - __LIBS_LIST__) / 8 - 2)
    KEEP(*(SORT_BY_INIT_PRIORITY(.aros.set.LIBS.*)))
    KEEP(*(.aros.set.LIBS*))
    QUAD(0)
    __LIBS_END__ = .;
    __INITLIB_LIST__ = .;
    QUAD((__INITLIB_END__ - __INITLIB_LIST__) / 8 - 2)
    KEEP (*(SORT_BY_INIT_PRIORITY(.aros.set.INITLIB.*)))
    KEEP (*(.aros.set.INITLIB*))
    QUAD(0)
    __INITLIB_END__ = .;
  }
}
