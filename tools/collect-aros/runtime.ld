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
    __RELLIBS_LIST__ = .;
    QUAD((__RELLIBS_END__ - __RELLIBS_LIST__) / 8 - 2)
    KEEP(*(SORT_BY_INIT_PRIORITY(.aros.set.RELLIBS.*)))
    KEEP(*(.aros.set.RELLIBS*))
    QUAD(0)
    __RELLIBS_END__ = .;
    __INIT_LIST__ = .;
    QUAD((__INIT_END__ - __INIT_LIST__) / 8 - 2)
    KEEP (*(SORT_BY_INIT_PRIORITY(.aros.set.INIT.*)))
    KEEP (*(.aros.set.INIT))
    QUAD(0)
    __INIT_END__ = .;
    __INITLIB_LIST__ = .;
    QUAD((__INITLIB_END__ - __INITLIB_LIST__) / 8 - 2)
    KEEP (*(SORT_BY_INIT_PRIORITY(.aros.set.INITLIB.*)))
    KEEP (*(.aros.set.INITLIB*))
    QUAD(0)
    __INITLIB_END__ = .;
    __OPENLIB_LIST__ = .;
    QUAD((__OPENLIB_END__ - __OPENLIB_LIST__) / 8 - 2)
    KEEP (*(SORT_BY_INIT_PRIORITY(.aros.set.OPENLIB.*)))
    KEEP (*(.aros.set.OPENLIB*))
    QUAD(0)
    __OPENLIB_END__ = .;
    __CLASSESINIT_LIST__ = .;
    QUAD((__CLASSESINIT_END__ - __CLASSESINIT_LIST__) / 8 - 2)
    KEEP (*(SORT_BY_INIT_PRIORITY(.aros.set.CLASSESINIT.*)))
    KEEP (*(.aros.set.CLASSESINIT*))
    QUAD(0)
    __CLASSESINIT_END__ = .;
  }
}
