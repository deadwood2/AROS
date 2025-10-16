/*
 * misc.c
 *
 * Author: ppessi <Pekka.Pessi@hut.fi>
 *
 * Copyright � 1993 AmiTCP/IP Group, <AmiTCP-group@hut.fi>
 *                  Helsinki University of Technology, Finland.
 */

#include <proto/exec.h>

#include "base.h"

/*
 * Free nodes of a list (they should all be allocated with AllocVec())
 */
void FreeListVec(struct NetInfoDevice *nid, struct List *list)
{
    struct Node *entry, *next;

    for (entry = list->lh_Head; next = entry->ln_Succ; entry = next) {
        Remove(entry);
        FreeVec(entry);
        entry = NULL;
    }
}

/*
 * Ensure that a node is in list
 * return node if it is, NULL otherwise
 */
struct Node *FindNode(struct List *list, struct Node *node)
{
    struct Node *entry, *next;

    for (entry = list->lh_Head; next = entry->ln_Succ; entry = next)
        if (entry == node) {
            return node;
        }

    return NULL;
}

/*
 * Get next token from string *stringp, where tokens are nonempty
 * strings separated by characters from delim.
 *
 * Writes NULs into the string at *stringp to end tokens.
 * delim need not remain constant from call to call.
 * On return, *stringp points past the last NUL written (if there might
 * be further tokens), or is NULL (if there are definitely no more tokens).
 *
 * If *stringp is NULL, strtoken returns NULL.
 */
char *strsep(register char **stringp, register const char *delim)
{
    register char *s;
    register const char *spanp;
    register int c, sc;
    char *tok;

    if ((s = *stringp) == NULL)
        return (NULL);
    for (tok = s;;) {
        c = *s++;
        spanp = delim;
        do {
            if ((sc = *spanp++) == c) {
                if (c == 0)
                    s = NULL;
                else
                    s[-1] = 0;
                *stringp = s;
                return (tok);
            }
        } while (sc != 0);
    }
}
