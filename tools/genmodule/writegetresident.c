/*
    Copyright © 2018, The AROS Development Team. All rights reserved.

    Desc: function to write modulename_getresident.c. Part of genmodule.
*/
#include "genmodule.h"

void writegetresident(struct config *cfg)
{
    FILE *out;
    char line[256];

    snprintf(line, 255, "%s/%s_getresident.c", cfg->gendir, cfg->modulename);
    out = fopen(line, "w");
    if (out==NULL)
    {
        fprintf(stderr, "Could not write %s\n", line);
        exit(20);
    }
    if (!(cfg->options & OPTION_NORESIDENT))
    {
        fprintf(out,
                    "#include <proto/exec.h>\n"
                    "#include \"%s_libdefs.h\"\n"
                    "\n"
                    "#undef SysBase"
                    "\n"
                    "extern struct Resident GM_UNIQUENAME(ROMTag);\n"
                    "__attribute__((visibility(\"default\"))) APTR __get_resident()\n"
                    "{\n"
                    "    return &GM_UNIQUENAME(ROMTag);\n"
                    "}\n",
                    cfg->modulename
        );
    }
    fclose(out);
}
