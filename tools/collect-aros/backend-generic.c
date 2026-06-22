/*
    Copyright (C) 1995-2023, The AROS Development Team. All rights reserved.
*/

#include "misc.h"
#include "backend.h"
#include <sys/param.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include "env.h"

static FILE *my_popen(const char *command, const char *file)
{
    static char command_buf[MAXPATHLEN];

    size_t command_len = strlen(command);
    size_t file_len    = strlen(file);

    FILE *pipe;

    if (command_len >= sizeof(command_buf) - 1)
        fatal("collect_sets()", strerror(ENAMETOOLONG));
    memcpy(command_buf, command, command_len);
    if (command[command_len - 1] != ' ')
    {
        command_buf[command_len++] = ' ';
    }
    if (file_len + command_len >= sizeof(command_buf))
        fatal("collect_sets()", strerror(ENAMETOOLONG));
    memcpy(command_buf + command_len, file, file_len + 1);

    set_compiler_path();

    pipe = popen(command_buf, "r");
    if (pipe == NULL)
        fatal(command_buf, strerror(errno));

    return pipe;
}

/*
    The following routines are slow, but do the work and are the simplest to write down.
    All this will get integrated into the linker anyway, so there's no point
    in doing optimizations
*/
/* AROS: objdump crashes at its own startup on this target (posixc path
   translation heap fault), so we parse the ELF section headers directly
   here instead of spawning "objdump -h".  We only need section names. */
#include <stdint.h>

typedef struct {
    unsigned char e_ident[16];
    uint16_t e_type, e_machine;
    uint32_t e_version;
    uint64_t e_entry, e_phoff, e_shoff;
    uint32_t e_flags;
    uint16_t e_ehsize, e_phentsize, e_phnum, e_shentsize, e_shnum, e_shstrndx;
} Elf64_Ehdr_t;

typedef struct {
    uint32_t sh_name;
    uint32_t sh_type;
    uint64_t sh_flags, sh_addr, sh_offset, sh_size;
    uint32_t sh_link, sh_info;
    uint64_t sh_addralign, sh_entsize;
} Elf64_Shdr_t;

void collect_sets(const char *file, setnode **setlist_ptr)
{
    Elf64_Ehdr_t ehdr;
    Elf64_Shdr_t *shdrs = NULL;
    char *shstr = NULL;
    char secname[201];
    FILE *f;
    size_t shtblsize, i;

    f = fopen(file, "rb");
    if (f == NULL)
        fatal(file, strerror(errno));

    if (fread(&ehdr, 1, sizeof(ehdr), f) != sizeof(ehdr)
        || ehdr.e_ident[0] != 0x7f || ehdr.e_ident[1] != 'E'
        || ehdr.e_ident[2] != 'L'  || ehdr.e_ident[3] != 'F'
        || ehdr.e_ident[4] != 2 /* ELFCLASS64 */
        || ehdr.e_shoff == 0 || ehdr.e_shnum == 0)
    {
        fclose(f);
        fatal(file, "not a 64-bit ELF object (collect_sets)");
    }

    /* The partial-link output is always elf64 here. */
    parse_format("elf64");

    shtblsize = (size_t)ehdr.e_shnum * sizeof(Elf64_Shdr_t);
    shdrs = malloc(shtblsize);
    if (shdrs == NULL || fseek(f, (long)ehdr.e_shoff, SEEK_SET) != 0
        || fread(shdrs, 1, shtblsize, f) != shtblsize)
    {
        free(shdrs); fclose(f);
        fatal(file, "cannot read section headers (collect_sets)");
    }

    /* Load the section-header string table. */
    {
        Elf64_Shdr_t *str = &shdrs[ehdr.e_shstrndx];
        shstr = malloc((size_t)str->sh_size);
        if (shstr == NULL || fseek(f, (long)str->sh_offset, SEEK_SET) != 0
            || fread(shstr, 1, (size_t)str->sh_size, f) != (size_t)str->sh_size)
        {
            free(shstr); free(shdrs); fclose(f);
            fatal(file, "cannot read section string table (collect_sets)");
        }
    }

    for (i = 0; i < ehdr.e_shnum; i++)
    {
        const char *nm = shstr + shdrs[i].sh_name;
        size_t l = strlen(nm);
        if (l == 0 || l > 200)
            continue;
        memcpy(secname, nm, l + 1);
        parse_secname(secname, setlist_ptr);
    }

    free(shstr);
    free(shdrs);
    fclose(f);
}

void collect_libs(const char *file, setnode **liblist_ptr)
{
    char secname[201];
    char buff[256];
    unsigned long offset;
    char type;

    FILE *pipe = my_popen(NM_NAME " ", file);

    while (fgets(buff, sizeof(buff), pipe)) {
        struct setnode *node;
        int pri;

        offset = 0;

        if (sscanf(buff, "%lx %c %200s ", &offset, &type, secname) != 3 &&
            sscanf(buff, " %c %200s", &type, secname) != 2)
            continue;

        if (strncmp(secname, "__aros_libreq_", 14) != 0)
            continue;

        if (type == 'A') {
            char *cp, *tmp;

            cp = strchr(secname + 14, '.');
            if (cp == NULL)
                continue;

            pri = strtoul(cp+1, &tmp, 0);
            if ((cp+1) == tmp)
                continue;

            *(cp++) = 0;
        } else if (type == 'w') {
            pri = 0;
        } else {
            continue;
        }

        node = calloc(sizeof(*node),1);
        node->secname = strdup(secname);
        node->off_setname = 14;
        node->pri = pri;
        node->next = *liblist_ptr;
        *liblist_ptr = node;
    }

    pclose(pipe);
}

void collect_extra(const char *file, setnode **liblist_ptr)
{
    char *objname, secname[201];
    char buff[256];
    unsigned long offset;
    char type;

    FILE *pipe = my_popen(NM_NAME " ", file);

    while (fgets(buff, sizeof(buff), pipe)) {
        struct setnode *node;

        offset = 0;

        if (sscanf(buff, "%lx %c %200s ", &offset, &type, secname) != 3 &&
            sscanf(buff, " %c %200s", &type, secname) != 2)
            continue;

        if ((strncmp(secname, "__cxa_pure_virtual", 18) == 0) &&
            (type == 'w'))
        {
            objname = calloc(strlen(OBJLIBDIR)+strlen(AROSOBJ_CXXPUREVIRT)+2, 1);
            sprintf(objname, "%s/%s", OBJLIBDIR, AROSOBJ_CXXPUREVIRT);
        }
        else
            continue;

        node = calloc(sizeof(*node),1);
        node->secname = strdup(objname);
        node->next = *liblist_ptr;
        *liblist_ptr = node;
    }

    pclose(pipe);
}

int check_and_print_undefined_symbols(const char *file)
{
    char buf[200];
    int undefined_syms = 0;
    size_t cnt;

    strcpy(buf, NM_NAME);
    if (!strstr(buf, "--demangle"))
        strcat(buf, " --demangle");
    if (!strstr(buf, "--undefined-only"))
        strcat(buf, " --undefined-only");
    if ((have_gnunm) && (!strstr(buf, "--line-numbers")))
        strcat(buf, " --line-numbers");

    FILE *pipe = my_popen(buf, file);

    while ((cnt = fread(buf, 1, sizeof(buf), pipe)) != 0)
    {
        if (!undefined_syms)
        {
            undefined_syms = 1;
            fprintf(stderr, "There are undefined symbols in '%s':\n", file);
        }

        fwrite(buf, cnt, 1, stderr);
    }

    pclose(pipe);

    return undefined_syms;
}

void backend_init(char *ldname)
{
    // nothing to do
    return;
}
