# Makefile for C++ project with auto-dependencies and multiple build options
# Copyright (C) 2000-2003 Marc Mongenet
#
# mods by Eric Peters, this is not an original
# last mod date 03/07/19

###############################################################################
# USER DEFINED VARIABLES
# The following variable values must be suited to the build environment.
# Use FLAGS_FILE to set these settings without having to change the 
# Makefile
#
# Use PLATFORM_RULES to make changes to the makefile that are build machine
# Dependant.  The platform file shouldn't be checked in
#
# Use TARGETS_FILE to define addtional targets that require special handling
#    Additional variables to use with TARGETS_FILE:
#      ADD_DEPS    : names of additional dependancies
#      ADD_CLEAN   : addtional files/directories to remove with "make clean"
#      ADD_CLOBBER : addtional files/directories to remove with "make clobber"
###############################################################################

AT=@
FLAGS_FILE=Makefile.flags
PLATFORM_FILE=Makefile.platform
TARGETS_FILE=Makefile.targets

USER_OPTS=
USER_LDOPTS=

# source files suffix (all source files must have the same suffix)
SOURCE_SUFFIX = cpp

# C++ compiler
CXX ?= g++

# source files directory
#srcdir = .
#srcdir = src
srcdir = $(if $(wildcard src),src,.)

# build directory
builddir = .build

# preprocessor options to find all included files
INC_PATH = -I$(if $(wildcard include),include,$(srcdir))

# name of the file to build
#EXE?=$(notdir $(basename $(shell grep -l "\(int\|void\) \+main *(.*)" $(srcdir)/*$(SOURCE_SUFFIX) 2> /dev/null )))
EXE?=$(shell basename ${CURDIR})

# libraries link options ('-lm' is common to link with the math library)
#LNK_LIBS =

# other compilation options
COMPILE_OPTS =-std=c++11

# basic compiler warning options (for GOAL_EXE)
#BWARN_OPTS = -Wall -ansi
BWARN_OPTS = -Wall

# extented compiler warning options (for GOAL_DEBUG)
# ESP: removed -pedantic as it's a pita
# ESP: removed  -Wtraditional  -Wstrict-prototypes  -Wnested-externs as they aren't valid for C++
EWARN_OPTS = $(BWARN_OPTS) \
	     -Wshadow -Wpointer-arith -Wcast-qual -Wcast-align\
	     -Wwrite-strings -Wredundant-decls -Woverloaded-virtual -Winline

###############################################################################
# INTERNAL VARIABLES
# The following variable values should not depend on the build environment.
# You may safely stop the configuration at this line.
###############################################################################

# You may freely change the following goal names if you dislike them.
GOAL_DEBUG = debug
GOAL_PROF = prof
GOAL_EXE = all

ifeq "${MAKECMDGOALS}" ""
	MAKECMDGOALS=${GOAL_EXE}
endif

# build options for GOAL_DEBUG (executable for debugging) goal

ifeq "$(MAKECMDGOALS)" "$(GOAL_DEBUG)"

 # specific options for debugging
 GOAL_OPTS = -g -rdynamic
 # compilation verification options
 WARN_OPTS = $(EWARN_OPTS)
 # optimization options
 OPTIMISE_OPTS =
 # dependencies must be up to date
 CHECK_DEPS = yes

else

 # build options for GOAL_PROF (executable for profiling) goal
 ifeq "$(MAKECMDGOALS)" "$(GOAL_PROF)"

  # specific options for profiling
  GOAL_OPTS = -pg
  # compilation verification options
  WARN_OPTS = $(BWARN_OPTS)
  # optimization options
  OPTIMISE_OPTS = -O2
  # dependencies must be up to date
  CHECK_DEPS = yes

 else

  # build options for GOAL_EXE (optimized executable) goal
  ifeq "$(MAKECMDGOALS)" "$(GOAL_EXE)"

   # specific options for optimized executable
   GOAL_OPTS = 
   # compilation verification options
   WARN_OPTS = $(BWARN_OPTS)
   # optimization options
   OPTIMISE_OPTS = -O3 -fomit-frame-pointer
   # dependencies must be up to date
   CHECK_DEPS = yes

  else
   # Other goals do not require up to date dependencies.
   CHECK_DEPS = no

  endif
 endif
endif

# preprocessor options
CPPOPTS = $(INC_PATH)

# compiler options
CXXOPTS = $(GOAL_OPTS) $(COMPILE_OPTS) $(WARN_OPTS) $(OPTIMISE_OPTS) $(USER_OPTS)

# linker options
LDOPTS = $(GOAL_OPTS) $(LNK_LIBS) $(USER_LDOPTS)

# source files in this project
sources := $(wildcard $(srcdir)/*.$(SOURCE_SUFFIX))

# object files in this project
objs := $(notdir $(sources))
objs := $(addprefix $(builddir)/, $(objs))
objs := $(objs:.$(SOURCE_SUFFIX)=.o)

# executable with full path
#exe = $(builddir)/$(EXE)
exe = $(EXE)

.DEFAULT: $(GOAL_EXE)

# This makefile creates and includes makefiles containing actual dependencies.
# For every source file a dependencies makefile is created and included.
# The deps variable contains the list of all dependencies makefiles.
deps_suffix = d
deps := $(objs:.o=.$(deps_suffix))

# To detect goal changes (for instance from GOAL_DEBUG to GOAL_EXE)
# between invocations, this makefile creates an empty file (the goal flag
# file) which suffix is the goal name.
goal_flag_file_prefix = $(builddir)/Last_make_goal_was_
goal_flag_file = $(goal_flag_file_prefix)$(MAKECMDGOALS)

-include $(FLAGS_FILE)
-include $(PLATFORM_FILE)

###############################################################################
# TARGETS
###############################################################################

# Delete the target file of a rule if the command used to update it failed.
# Do that because the newly generated target file may be corrupted but appear
# up to date.
.DELETE_ON_ERROR:

# Clear default suffix list to disable all implicit rules.
.SUFFIXES:


# If source files exist then build the EXE file.
.PHONY:	$(GOAL_EXE)
ifneq "$(strip $(sources))" ""
$(GOAL_EXE):	$(shell mkdir -p $(builddir)) $(exe)
else
$(GOAL_EXE):
	@echo "No source file found."
endif

help: usage

-include $(TARGETS_FILE)

# usage message for this makefile
.PHONY:	usage
usage:
	@echo "GOAL		EFFECT"
	@echo "----		------"
	@echo "usage		print this message"
	@echo "list		list the source files"
	@echo "$(GOAL_EXE)		build the executable"
	@echo "$(GOAL_DEBUG)		build the executable with debug options"
	@echo "$(GOAL_PROF)		build the executable with profiling options"
	@echo "clean		remove all object and executable files"
	@echo "tags		create ctags for project(requires ctags exe in path)"
	@echo "clobber	        remove all object/executable/tag files"
	@echo "make_flags	save a Makefile.flags example"
	@echo "make_targets	save a Makefile.targets example"

.PHONY:	tags
tags:
	@if ! which ctags >/dev/null 2>&1 ; then \
		echo "ctags not found in path" ;\
	else \
		echo "Making tags..." ;\
		rm -f tags ;\
		ctags --file-scope=yes -R --extra=+f+q --fields=+ias --c++-kinds=+p ;\
		sed -i -e "1 i!_TAG_COMMAND_LINE [ctags --file-scope=yes -R --extra=+f+q --fields=+ias --c++-kinds=+p]" tags;\
	fi

.PHONY:	make_flags
make_flags:
	@if [ ! -f $(FLAGS_FILE) ] ; then \
		echo "#BOOST_LIB_DIR = /usr/lib" > $(FLAGS_FILE) ;\
		echo "#LNK_LIBS = \$${BOOST_LIB_DIR}/libboost_thread.a \\" >> $(FLAGS_FILE) ;\
		echo "           \$${BOOST_LIB_DIR}/libboost_filesystem.a \\" >> $(FLAGS_FILE) ;\
		echo "           \$${BOOST_LIB_DIR}/libboost_system.a \\" >> $(FLAGS_FILE) ;\
		echo "           \$${BOOST_LIB_DIR}/libboost_regex.a \\" >> $(FLAGS_FILE) ;\
		echo "           \$${BOOST_LIB_DIR}/libboost_log_setup.a \\" >> $(FLAGS_FILE) ;\
		echo "           \$${BOOST_LIB_DIR}/libboost_log.a \\" >> $(FLAGS_FILE) ;\
		echo "           -lsqlite3 \\" >> $(FLAGS_FILE) ;\
		echo "           -lpthread \\" >> $(FLAGS_FILE) ;\
		echo "           -ldl \\" >> $(FLAGS_FILE) ;\
		echo "           -lrt" >> $(FLAGS_FILE) ;\
		echo "" >> $(FLAGS_FILE) ;\
		echo "#EXTRA_FILES=catalog.db3 catalog.log" >> $(FLAGS_FILE) ;\
		echo "#CXXFLAGS=-std=c++0x" >> $(FLAGS_FILE) ;\
		echo "#CXXFLAGS=-std=c++11" >> $(FLAGS_FILE) ;\
		echo "" >> $(FLAGS_FILE) ;\
		echo "#USER_OPTS=" >> $(FLAGS_FILE) ;\
		echo "#USER_LDOPTS=" >> $(FLAGS_FILE) ;\
		echo "" >> $(FLAGS_FILE) ;\
		echo "# vim: noet" >> $(FLAGS_FILE) ;\
	else \
		echo "$(FLAGS_FILE) already exists" ;\
	fi

.PHONY:	make_targets

make_targets:
	@if [ ! -f $(TARGETS_FILE) ] ; then \
		echo "#SHELL=/bin/bash" > $(TARGETS_FILE) ;\
		echo "" >> $(TARGETS_FILE) ;\
		echo "#ADD_CLOBBER+=libpuzzle.a libpuzzle/libpuzzle-0.11 sqlite/sqlite3.o" >> $(TARGETS_FILE) ;\
		echo "#ADD_CLEAN+=picchk.db3" >> $(TARGETS_FILE) ;\
		echo "#ADD_DEPS+=libpuzzle.a .build/sqlite3.o" >> $(TARGETS_FILE) ;\
		echo "" >> $(TARGETS_FILE) ;\
		echo "#libpuzzle.a:" >> $(TARGETS_FILE) ;\
		echo "#	${AT}rm -rf libpuzzle/libpuzzle-0.11" >> $(TARGETS_FILE) ; \
		echo "#	${AT}tar zxvf ./libpuzzle/libpuzzle-0.11.tar.gz -C ./libpuzzle" >> $(TARGETS_FILE) ;\
		echo "#	${AT}cd libpuzzle/libpuzzle-0.11 && ./configure --disable-shared && make LDFLAGS+="-lm"" >> $(TARGETS_FILE) ;\
		echo "#	${AT}cp libpuzzle/libpuzzle-0.11/src/.libs/libpuzzle.a ." >> $(TARGETS_FILE) ;\
		echo "#	${AT}rm -rf libpuzzle/libpuzzle-0.11" >> $(TARGETS_FILE) ;\
		echo "" >> $(TARGETS_FILE) ;\
		echo "#.build/sqlite3.o:" >> $(TARGETS_FILE) ;\
		echo "#	${AT}cd sqlite3 && make" >> $(TARGETS_FILE) ;\
		echo "#	${AT}cp sqlite3/sqlite3.o .build" >> $(TARGETS_FILE) ;\
		echo "" >> $(TARGETS_FILE) ;\
		echo "# vim: noet" >> $(TARGETS_FILE) ;\
	else \
		echo "$(TARGETS_FILE) already exists" ;\
	fi


# GOAL_DEBUG and GOAL_PROF targets use the same rules than GOAL_EXE.
.PHONY:	$(GOAL_DEBUG)
$(GOAL_DEBUG):	$(GOAL_EXE)

.PHONY:	$(GOAL_PROF)
$(GOAL_PROF):	$(GOAL_EXE)

###############################################################################
# BUILDING
# Note: CPPFLAGS, CXXFLAGS or LDFLAGS are not used but may be specified by the
# user at make invocation.
###############################################################################

# linking
$(exe):	$(ADD_DEPS) $(objs)
	${AT} echo "Linking ${exe}"
	${AT} $(CXX) $^ -o $@ $(LDOPTS) $(LDFLAGS)

# explicit definition of the implicit rule used to compile source files
$(builddir)/%.o: $(srcdir)/%.$(SOURCE_SUFFIX)
	${AT} echo "Compiling $<"
	${AT} $(CXX) -c $< $(CPPOPTS) $(CXXOPTS) $(CPPFLAGS) $(CXXFLAGS) -o $@

# Rule to build our included dependencies makefiles.
# This rule is used by GNU Make because it automatically tries to (re)build
# obsolete or non-existent included makefiles.
# These files are created with one line of the form:
# 1.o 1.d: $(goal_flag_file) 1.cc 1.h 2.h 3.h g.h
# The implicit rule previously defined will be used for compilation.
# Note that the dependencies can be goal specific.
# The goal_flag_file is determined at run time because it must be the current
# goal and not the goal in use when the dependencies makefile was created.
$(builddir)/%.$(deps_suffix):	$(srcdir)/%.$(SOURCE_SUFFIX) $(goal_flag_file)
	${AT} $(SHELL) -ec '$(CXX) -MM $(CPPOPTS) $(CPPFLAGS) $(CXXFLAGS) $(USER_OPTS) $< |\
	sed '\''s@\($*\)\.o[ :]*@$(builddir)/\1.o $@: $$(goal_flag_file) @g'\'' > $@;\
	[ -s $@ ] || rm -f $@'

# If dependencies have to be up to date then include dependencies makefiles.
ifeq "$(CHECK_DEPS)" "yes"
 ifneq "$(strip $(sources))" ""
   -include $(deps)
  endif
endif

# Rule to produce the goal flag file.
# If the goal has changed then we must rebuild on a clean state because
# pre-processor DEFINE's may have changed.
$(goal_flag_file):
	${AT} touch ${goal_flag_file_prefix}PHONY
	${AT} rm -f $(exe) $(goal_flag_file_prefix)* $(objs) $(deps) 
	${AT} touch $@

###############################################################################
# NON-BUILD TARGETS
###############################################################################

# List the source files
.PHONY:	list
list:
	${AT} echo $(sources) | tr [:space:] \\n

.PHONY:	vars
vars:
	${AT} echo "INC_PATH = ${INC_PATH}"

# Remove all files that are normally created by building the program.
.PHONY:	clean
clean:
	${AT} echo "Cleaning..."
	${AT} rm -f $(exe) ${EXTRA_FILES}
	${AT} rm -rf $(builddir) ${ADD_CLEAN}

.PHONY:	clobber
clobber: clean
	${AT} rm -rf tags ${ADD_CLOBBER}

