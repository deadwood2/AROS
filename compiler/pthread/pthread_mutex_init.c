/*
  Copyright (C) 2014 Szilard Biro
  Copyright (C) 2018 Harry Sintonen
  Copyright (C) 2019 Stefan "Bebbo" Franke - AmigaOS 3 port

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

#include "pthread_intern.h"
#include "debug.h"

int _pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr, BOOL staticinit)
{
    DB2(bug("%s(%p, %p)\n", __FUNCTION__, mutex, attr));

    if (mutex == NULL)
        return EINVAL;

    if (attr)
        mutex->kind = attr->kind;
    else if (!staticinit)
        mutex->kind = PTHREAD_MUTEX_DEFAULT;
    InitSemaphore(&mutex->semaphore);
    mutex->incond = 0;

    return 0;
}

int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr)
{
    D(bug("%s(%p, %p)\n", __FUNCTION__, mutex, attr));

    return _pthread_mutex_init(mutex, attr, FALSE);
}
