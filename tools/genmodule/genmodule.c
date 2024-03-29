/*
    Copyright (C) 1995-2020, The AROS Development Team. All rights reserved.

    Main for genmodule. A tool to generate files for building modules.
*/
#include "genmodule.h"

int main(int argc, char **argv)
{
    char *s;
    struct config *cfg = initconfig(argc, argv);

    switch (cfg->command)
    {
    case FILES:
        writestart(cfg);
        writeend(cfg);
        if (cfg->options & OPTION_AUTOINIT)
        {
            writeautoinit(cfg, 0); /* normal */
            if (cfg->options & OPTION_RELLINKLIB)
                writeautoinit(cfg, 1); /* relbase */
        }
        if (cfg->modtype == LIBRARY)
        {
            writegetlibbase(cfg, 0); /* normal */
            if (cfg->options & OPTION_RELLINKLIB)
                writegetlibbase(cfg, 1); /* relbase */
        }
        if (cfg->options & OPTION_STUBS)
        {
            writestubs(cfg, 0); /* normal */
            if (cfg->options & OPTION_RELLINKLIB)
                writestubs(cfg, 1); /* relbase */
        }
        writelinkentries(cfg);
        break;

    case INCLUDES:
        if (cfg->options & OPTION_INCLUDES)
        {
            writeincproto(cfg);
            writeincclib(cfg);
            writeincdefines(cfg);
            writeincinline(cfg);
        }
        if (cfg->interfacelist)
            writeincinterfaces(cfg);
        break;

    case LIBDEFS:
        writeinclibdefs(cfg);
        break;

    case MAKEFILE:
        writemakefile(cfg);
        break;

    case WRITEFUNCLIST:
        /* Ignore the functionlist and the methodlist that are available in the
         * .conf file.
         */
        cfg->funclist = NULL;
        if (cfg->classlist != NULL)
            cfg->classlist->methlist = NULL;

        writefunclist(cfg);
        break;

    case WRITEFD:
        if ((cfg->options & OPTION_INCLUDES) && (cfg->funclist != NULL))
        {
            writefd(cfg);
        }
        break;

    case WRITESKEL:
        writeskel(cfg);
        break;

    case WRITETHUNK:
        writethunk(cfg);
        break;

    default:
        fprintf(stderr, "Internal error in main: Unhandled command type\n");
        exit(20);
    }

    return 0;
}
