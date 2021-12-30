#   Copyright © 2002-2009, The AROS Development Team. All rights reserved.
#   $Id$
#
#   Desc: Makefile to create genmodule
#

USER_CFLAGS := -Wall -Werror -Wunused -O2

-include $(TOP)/config/make.cfg
include Makefile.deps

HOST_CC	       ?= gcc
HOST_STRIP     ?= strip
MECHO	       ?= echo
HOST_CFLAGS    ?= $(USER_CFLAGS)
GENMODULE      ?= genmodule
GM_CPPFLAGS    :=

ifeq ($(AROS_TARGET_CPU),x86_64)
GM_CPPFLAGS    := -DLIBBASE_IN_REG
endif

all : $(GENMODULE)

$(GENMODULE) : $(GENMODULE_DEPS)
	@$(MECHO) "Compiling $(notdir $@)..."
	@$(HOST_CC) -g $(HOST_CFLAGS) $(GM_CPPFLAGS) -I$(GENINCDIR) -I$(TOP)/$(CURDIR) $(GENMODULE_SRCS) -o $@
#	@$(HOST_CC) -E $(HOST_CFLAGS) $(GM_CPPFLAGS) -I$(GENINCDIR) -I$(TOP)/$(CURDIR) $(GENMODULE_SRCS) -o $@.C
#	@$(HOST_STRIP) $@

clean :
	@$(RM) $(GENMODULE)

.PHONY : all clean
