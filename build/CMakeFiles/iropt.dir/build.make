# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


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
CMAKE_SOURCE_DIR = /home/comtest/Desktop/compile/Compiler2023-yfscc

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/comtest/Desktop/compile/Compiler2023-yfscc/build

# Include any dependencies generated for this target.
include CMakeFiles/iropt.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/iropt.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/iropt.dir/flags.make

CMakeFiles/iropt.dir/opt/PassManager.cc.o: CMakeFiles/iropt.dir/flags.make
CMakeFiles/iropt.dir/opt/PassManager.cc.o: ../opt/PassManager.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/comtest/Desktop/compile/Compiler2023-yfscc/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/iropt.dir/opt/PassManager.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/iropt.dir/opt/PassManager.cc.o -c /home/comtest/Desktop/compile/Compiler2023-yfscc/opt/PassManager.cc

CMakeFiles/iropt.dir/opt/PassManager.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/iropt.dir/opt/PassManager.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/comtest/Desktop/compile/Compiler2023-yfscc/opt/PassManager.cc > CMakeFiles/iropt.dir/opt/PassManager.cc.i

CMakeFiles/iropt.dir/opt/PassManager.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/iropt.dir/opt/PassManager.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/comtest/Desktop/compile/Compiler2023-yfscc/opt/PassManager.cc -o CMakeFiles/iropt.dir/opt/PassManager.cc.s

CMakeFiles/iropt.dir/opt/CollectUsedGlobals.cc.o: CMakeFiles/iropt.dir/flags.make
CMakeFiles/iropt.dir/opt/CollectUsedGlobals.cc.o: ../opt/CollectUsedGlobals.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/comtest/Desktop/compile/Compiler2023-yfscc/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/iropt.dir/opt/CollectUsedGlobals.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/iropt.dir/opt/CollectUsedGlobals.cc.o -c /home/comtest/Desktop/compile/Compiler2023-yfscc/opt/CollectUsedGlobals.cc

CMakeFiles/iropt.dir/opt/CollectUsedGlobals.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/iropt.dir/opt/CollectUsedGlobals.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/comtest/Desktop/compile/Compiler2023-yfscc/opt/CollectUsedGlobals.cc > CMakeFiles/iropt.dir/opt/CollectUsedGlobals.cc.i

CMakeFiles/iropt.dir/opt/CollectUsedGlobals.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/iropt.dir/opt/CollectUsedGlobals.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/comtest/Desktop/compile/Compiler2023-yfscc/opt/CollectUsedGlobals.cc -o CMakeFiles/iropt.dir/opt/CollectUsedGlobals.cc.s

CMakeFiles/iropt.dir/opt/DataflowAnalysis.cc.o: CMakeFiles/iropt.dir/flags.make
CMakeFiles/iropt.dir/opt/DataflowAnalysis.cc.o: ../opt/DataflowAnalysis.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/comtest/Desktop/compile/Compiler2023-yfscc/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/iropt.dir/opt/DataflowAnalysis.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/iropt.dir/opt/DataflowAnalysis.cc.o -c /home/comtest/Desktop/compile/Compiler2023-yfscc/opt/DataflowAnalysis.cc

CMakeFiles/iropt.dir/opt/DataflowAnalysis.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/iropt.dir/opt/DataflowAnalysis.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/comtest/Desktop/compile/Compiler2023-yfscc/opt/DataflowAnalysis.cc > CMakeFiles/iropt.dir/opt/DataflowAnalysis.cc.i

CMakeFiles/iropt.dir/opt/DataflowAnalysis.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/iropt.dir/opt/DataflowAnalysis.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/comtest/Desktop/compile/Compiler2023-yfscc/opt/DataflowAnalysis.cc -o CMakeFiles/iropt.dir/opt/DataflowAnalysis.cc.s

CMakeFiles/iropt.dir/opt/DeadCodeElim.cc.o: CMakeFiles/iropt.dir/flags.make
CMakeFiles/iropt.dir/opt/DeadCodeElim.cc.o: ../opt/DeadCodeElim.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/comtest/Desktop/compile/Compiler2023-yfscc/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/iropt.dir/opt/DeadCodeElim.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/iropt.dir/opt/DeadCodeElim.cc.o -c /home/comtest/Desktop/compile/Compiler2023-yfscc/opt/DeadCodeElim.cc

CMakeFiles/iropt.dir/opt/DeadCodeElim.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/iropt.dir/opt/DeadCodeElim.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/comtest/Desktop/compile/Compiler2023-yfscc/opt/DeadCodeElim.cc > CMakeFiles/iropt.dir/opt/DeadCodeElim.cc.i

CMakeFiles/iropt.dir/opt/DeadCodeElim.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/iropt.dir/opt/DeadCodeElim.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/comtest/Desktop/compile/Compiler2023-yfscc/opt/DeadCodeElim.cc -o CMakeFiles/iropt.dir/opt/DeadCodeElim.cc.s

CMakeFiles/iropt.dir/opt/ConstantPropagation.cc.o: CMakeFiles/iropt.dir/flags.make
CMakeFiles/iropt.dir/opt/ConstantPropagation.cc.o: ../opt/ConstantPropagation.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/comtest/Desktop/compile/Compiler2023-yfscc/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/iropt.dir/opt/ConstantPropagation.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/iropt.dir/opt/ConstantPropagation.cc.o -c /home/comtest/Desktop/compile/Compiler2023-yfscc/opt/ConstantPropagation.cc

CMakeFiles/iropt.dir/opt/ConstantPropagation.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/iropt.dir/opt/ConstantPropagation.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/comtest/Desktop/compile/Compiler2023-yfscc/opt/ConstantPropagation.cc > CMakeFiles/iropt.dir/opt/ConstantPropagation.cc.i

CMakeFiles/iropt.dir/opt/ConstantPropagation.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/iropt.dir/opt/ConstantPropagation.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/comtest/Desktop/compile/Compiler2023-yfscc/opt/ConstantPropagation.cc -o CMakeFiles/iropt.dir/opt/ConstantPropagation.cc.s

# Object files for target iropt
iropt_OBJECTS = \
"CMakeFiles/iropt.dir/opt/PassManager.cc.o" \
"CMakeFiles/iropt.dir/opt/CollectUsedGlobals.cc.o" \
"CMakeFiles/iropt.dir/opt/DataflowAnalysis.cc.o" \
"CMakeFiles/iropt.dir/opt/DeadCodeElim.cc.o" \
"CMakeFiles/iropt.dir/opt/ConstantPropagation.cc.o"

# External object files for target iropt
iropt_EXTERNAL_OBJECTS =

libiropt.a: CMakeFiles/iropt.dir/opt/PassManager.cc.o
libiropt.a: CMakeFiles/iropt.dir/opt/CollectUsedGlobals.cc.o
libiropt.a: CMakeFiles/iropt.dir/opt/DataflowAnalysis.cc.o
libiropt.a: CMakeFiles/iropt.dir/opt/DeadCodeElim.cc.o
libiropt.a: CMakeFiles/iropt.dir/opt/ConstantPropagation.cc.o
libiropt.a: CMakeFiles/iropt.dir/build.make
libiropt.a: CMakeFiles/iropt.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/comtest/Desktop/compile/Compiler2023-yfscc/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Linking CXX static library libiropt.a"
	$(CMAKE_COMMAND) -P CMakeFiles/iropt.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/iropt.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/iropt.dir/build: libiropt.a

.PHONY : CMakeFiles/iropt.dir/build

CMakeFiles/iropt.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/iropt.dir/cmake_clean.cmake
.PHONY : CMakeFiles/iropt.dir/clean

CMakeFiles/iropt.dir/depend:
	cd /home/comtest/Desktop/compile/Compiler2023-yfscc/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/comtest/Desktop/compile/Compiler2023-yfscc /home/comtest/Desktop/compile/Compiler2023-yfscc /home/comtest/Desktop/compile/Compiler2023-yfscc/build /home/comtest/Desktop/compile/Compiler2023-yfscc/build /home/comtest/Desktop/compile/Compiler2023-yfscc/build/CMakeFiles/iropt.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/iropt.dir/depend

