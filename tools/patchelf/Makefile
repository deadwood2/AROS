# Copyright © 2022, The AROS Development Team. All rights reserved.
# $Id$

include $(TOP)/config/make.cfg

NOWARN_FLAGS := -Wall
USER_CXXFLAGS := -std=c++17

all : $(PATCHELF)

$(PATCHELF) : patchelf.cc
	@$(ECHO) "Compiling $(notdir $@)..."
	@$(HOST_CXX) $(HOST_CXXFLAGS) $(USER_CXXFLAGS) $(NOWARN_FLAGS) $< -o $@

clean:
	@$(RM) -f $(PATCHELF)
