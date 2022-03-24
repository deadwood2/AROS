SECTIONS
{
  .data :
  {
    . = ALIGN(8);
    HIDDEN(__LIBS_LIST__ = .);
    QUAD((__LIBS_END__ - __LIBS_LIST__) / 8 - 2)
    KEEP(*(.aros.set.LIBS))
    QUAD(0)
    HIDDEN(__LIBS_END__ = .);
    HIDDEN(__RELLIBS_LIST__ = .);
    QUAD((__RELLIBS_END__ - __RELLIBS_LIST__) / 8 - 2)
    KEEP(*(.aros.set.RELLIBS))
    QUAD(0)
    HIDDEN(__RELLIBS_END__ = .);
    HIDDEN(__INIT_LIST__ = .);
    QUAD((__INIT_END__ - __INIT_LIST__) / 8 - 2)
    KEEP (*(.aros.set.INIT))
    QUAD(0)
    HIDDEN(__INIT_END__ = .);
    HIDDEN(__INITLIB_LIST__ = .);
    QUAD((__INITLIB_END__ - __INITLIB_LIST__) / 8 - 2)
    KEEP (*(.aros.set.INITLIB))
    QUAD(0)
    HIDDEN(__INITLIB_END__ = .);
    HIDDEN(__OPENLIB_LIST__ = .);
    QUAD((__OPENLIB_END__ - __OPENLIB_LIST__) / 8 - 2)
    KEEP (*(.aros.set.OPENLIB))
    QUAD(0)
    HIDDEN(__OPENLIB_END__ = .);
    HIDDEN(__OPENDEV_LIST__ = .);
    QUAD((__OPENDEV_END__ - __OPENDEV_LIST__) / 8 - 2)
    KEEP (*(.aros.set.OPENDEV))
    QUAD(0)
    HIDDEN(__OPENDEV_END__ = .);
    HIDDEN(__CLOSEDEV_LIST__ = .);
    QUAD((__CLOSEDEV_END__ - __CLOSEDEV_LIST__) / 8 - 2)
    KEEP (*(.aros.set.CLOSEDEV))
    QUAD(0)
    HIDDEN(__CLOSEDEV_END__ = .);
    HIDDEN(__CLASSESINIT_LIST__ = .);
    QUAD((__CLASSESINIT_END__ - __CLASSESINIT_LIST__) / 8 - 2)
    KEEP (*(.aros.set.CLASSESINIT))
    QUAD(0)
    HIDDEN(__CLASSESINIT_END__ = .);
    HIDDEN(__PROGRAM_ENTRIES_LIST__ = .);
    QUAD((__PROGRAM_ENTRIES_END__ - __PROGRAM_ENTRIES_LIST__) / 8 - 2)
    KEEP (*(.aros.set.PROGRAM_ENTRIES))
    QUAD(0)
    HIDDEN(__PROGRAM_ENTRIES_END__ = .);
  }
}
