/*
    Copyright (C) 1995-2017, The AROS Development Team. All rights reserved.

    Desc: Function to write proto/modulename(_rel).h. Part of genmodule.
*/

#include "genmodule.h"

void writeincproto(struct config *cfg)
{
    FILE *out;
    char line[256], define[256], *banner;
    struct linelist *linelistit;

    snprintf(line, 255, "%s/proto/%s.h",
             cfg->gendir, cfg->includename
    );
    out = fopen(line, "w");

    if (out == NULL)
    {
        perror(line);
        exit(20);
    }

    banner = getBanner(cfg);
    fprintf(out,
            "#ifndef PROTO_%s_H\n"
            "#define PROTO_%s_H\n"
            "\n"
            "%s"
            "\n"
            , cfg->includenameupper
            , cfg->includenameupper
            , banner
    );
    fprintf(out,
            "#include <exec/types.h>\n"
            "%s"
            "#include <aros/system.h>\n"
            "\n"
            "#include <clib/%s_protos.h>\n"
            "\n",
            (cfg->modtype == DEVICE) ? "#include <exec/devices.h>\n" : "",
            cfg->includename
    );
    freeBanner(banner);
    if (cfg->options & OPTION_RELLINKLIB)
        fprintf(out,
                "#ifndef __%s_RELLIBBASE__\n",
                cfg->includenameupper
        );

    fprintf(out,
            " #if !defined(__NOLIBBASE__) && !defined(__%s_NOLIBBASE__)\n"
            "  #if !defined(%s)\n"
            "   #ifdef __%s_STDLIBBASE__\n"
            "    extern struct Library *%s;\n"
            "   #else\n"
            "    extern %s%s;\n"
            "   #endif\n"
            "  #endif\n"
            " #endif\n"
            " #ifndef __aros_getbase_%s\n"
            "  #define __aros_getbase_%s() (%s)\n"
            " #endif\n",
            cfg->includenameupper,
            cfg->libbase,
            cfg->includenameupper,
            cfg->libbase,
            cfg->libbasetypeptrextern, cfg->libbase,
            cfg->libbase,
            cfg->libbase, cfg->libbase
    );

    if (cfg->options & OPTION_RELLINKLIB)
        fprintf(out,
                "#else /* __%s_RELLIBASE__ */\n"
                " extern const IPTR __aros_rellib_offset_%s;\n"
                " #define AROS_RELLIB_OFFSET_%s __aros_rellib_offset_%s\n"
                " #define AROS_RELLIB_BASE_%s __aros_rellib_base_%s\n"
                " #ifndef __aros_getbase_%s\n"
                "  #ifndef __aros_getoffsettable\n"
                "   char *__aros_getoffsettable(void);\n"
                "  #endif\n"
                "  #define __aros_getbase_%s() (*(%s*)(__aros_getoffsettable()+__aros_rellib_offset_%s))\n"
                " #endif\n"
                "#endif\n",
                cfg->includenameupper,
                cfg->libbase,
                cfg->includenameupper, cfg->libbase,
                cfg->includenameupper, cfg->libbase,
                cfg->libbase,
                cfg->libbase, cfg->libbasetypeptrextern, cfg->libbase
        );

    fprintf(out, "\n");

    // define name must not start with a digit
    // this solves a problem with proto/8svx.h
    if (isdigit(cfg->includenameupper[0]))
    {
        snprintf(define, sizeof define, "X%s", cfg->includenameupper);
    }
    else
    {
        strncpy(define, cfg->includenameupper, sizeof define);
    }

    if (cfg->options & OPTION_RELLINKLIB)
    {
        fprintf(out,
                "#if !defined(NOLIBINLINE) && !defined(%s_NOLIBINLINE) && !defined(__%s_RELLIBBASE__)\n",
                define, cfg->includenameupper
        );
    }
    else
    {
        fprintf(out,
                "#if !defined(NOLIBINLINE) && !defined(%s_NOLIBINLINE)\n",
                define
        );
    }

    fprintf(out,
            "#   include <inline/%s.h>\n"
            "#elif !defined(NOLIBDEFINES) && !defined(%s_NOLIBDEFINES)\n"
            "#   include <defines/%s.h>\n"
            "#endif\n"
            "\n"
            "#endif /* PROTO_%s_H */\n",
            cfg->includename,
            define,
            cfg->includename,
            cfg->includenameupper
    );

    fclose(out);
}
