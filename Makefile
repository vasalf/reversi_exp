# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.9

# Default target executed when no arguments are given to make.
default_target: all

.PHONY : default_target

# Allow only one "make -f Makefile2" at a time, but pass parallelism.
.NOTPARALLEL:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/platypus/git/reversi_exp

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/platypus/git/reversi_exp

#=============================================================================
# Targets provided globally by CMake.

# Special rule for the target rebuild_cache
rebuild_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake to regenerate build system..."
	/usr/bin/cmake -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : rebuild_cache

# Special rule for the target rebuild_cache
rebuild_cache/fast: rebuild_cache

.PHONY : rebuild_cache/fast

# Special rule for the target edit_cache
edit_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "No interactive CMake dialog available..."
	/usr/bin/cmake -E echo No\ interactive\ CMake\ dialog\ available.
.PHONY : edit_cache

# Special rule for the target edit_cache
edit_cache/fast: edit_cache

.PHONY : edit_cache/fast

# Special rule for the target test
test:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running tests..."
	/usr/bin/ctest --force-new-ctest-process $(ARGS)
.PHONY : test

# Special rule for the target test
test/fast: test

.PHONY : test/fast

# The main all target
all: cmake_check_build_system
	$(CMAKE_COMMAND) -E cmake_progress_start /home/platypus/git/reversi_exp/CMakeFiles /home/platypus/git/reversi_exp/CMakeFiles/progress.marks
	$(MAKE) -f CMakeFiles/Makefile2 all
	$(CMAKE_COMMAND) -E cmake_progress_start /home/platypus/git/reversi_exp/CMakeFiles 0
.PHONY : all

# The main clean target
clean:
	$(MAKE) -f CMakeFiles/Makefile2 clean
.PHONY : clean

# The main clean target
clean/fast: clean

.PHONY : clean/fast

# Prepare targets for installation.
preinstall: all
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall

# Prepare targets for installation.
preinstall/fast:
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall/fast

# clear depends
depend:
	$(CMAKE_COMMAND) -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 1
.PHONY : depend

#=============================================================================
# Target rules for targets named reversi

# Build rule for target.
reversi: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 reversi
.PHONY : reversi

# fast build rule for target.
reversi/fast:
	$(MAKE) -f reversi/CMakeFiles/reversi.dir/build.make reversi/CMakeFiles/reversi.dir/build
.PHONY : reversi/fast

#=============================================================================
# Target rules for targets named strategy

# Build rule for target.
strategy: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 strategy
.PHONY : strategy

# fast build rule for target.
strategy/fast:
	$(MAKE) -f strategy/CMakeFiles/strategy.dir/build.make strategy/CMakeFiles/strategy.dir/build
.PHONY : strategy/fast

#=============================================================================
# Target rules for targets named genetics

# Build rule for target.
genetics: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 genetics
.PHONY : genetics

# fast build rule for target.
genetics/fast:
	$(MAKE) -f genetics/CMakeFiles/genetics.dir/build.make genetics/CMakeFiles/genetics.dir/build
.PHONY : genetics/fast

#=============================================================================
# Target rules for targets named gothello

# Build rule for target.
gothello: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 gothello
.PHONY : gothello

# fast build rule for target.
gothello/fast:
	$(MAKE) -f gothello/CMakeFiles/gothello.dir/build.make gothello/CMakeFiles/gothello.dir/build
.PHONY : gothello/fast

#=============================================================================
# Target rules for targets named reversi-cl

# Build rule for target.
reversi-cl: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 reversi-cl
.PHONY : reversi-cl

# fast build rule for target.
reversi-cl/fast:
	$(MAKE) -f reversi-cl/CMakeFiles/reversi-cl.dir/build.make reversi-cl/CMakeFiles/reversi-cl.dir/build
.PHONY : reversi-cl/fast

#=============================================================================
# Target rules for targets named reversigp

# Build rule for target.
reversigp: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 reversigp
.PHONY : reversigp

# fast build rule for target.
reversigp/fast:
	$(MAKE) -f reversigp/CMakeFiles/reversigp.dir/build.make reversigp/CMakeFiles/reversigp.dir/build
.PHONY : reversigp/fast

#=============================================================================
# Target rules for targets named test_genotype

# Build rule for target.
test_genotype: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 test_genotype
.PHONY : test_genotype

# fast build rule for target.
test_genotype/fast:
	$(MAKE) -f test/CMakeFiles/test_genotype.dir/build.make test/CMakeFiles/test_genotype.dir/build
.PHONY : test_genotype/fast

# Help Target
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... depend"
	@echo "... rebuild_cache"
	@echo "... edit_cache"
	@echo "... test"
	@echo "... reversi"
	@echo "... strategy"
	@echo "... genetics"
	@echo "... gothello"
	@echo "... reversi-cl"
	@echo "... reversigp"
	@echo "... test_genotype"
.PHONY : help



#=============================================================================
# Special targets to cleanup operation of make.

# Special rule to run CMake to check the build system integrity.
# No rule that depends on this can have commands that come from listfiles
# because they might be regenerated.
cmake_check_build_system:
	$(CMAKE_COMMAND) -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 0
.PHONY : cmake_check_build_system

