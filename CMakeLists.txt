PROJECT(playboy)

CMAKE_MINIMUM_REQUIRED(VERSION 3.16)

SET(CMAKE_C_FLAGS_DEBUG "-O0 -Wall -g -ggdb")
SET(CMAKE_C_FLAGS_RELEASE "-O3 -Wall")
SET(CMAKE_CXX_STANDARD 14)
SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -Wall -g -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wall")

SET(CMAKE_BUILD_TYPE Release)
SET(EXECUTABLE_OUTPUT_PATH bin)
SET(LIBRARY_OUTPUT_PATH lib)

ADD_DEFINITIONS(-DSIMPLE_MESSAGE)

INCLUDE_DIRECTORIES(pkg/include)

LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/pkg/lib)

###############################################################################
##############################  module  #######################################
###############################################################################

# AUX_SOURCE_DIRECTORY(imgui/ IMGUI)
# ADD_LIBRARY(imgui ${IMGUI})


###############################################################################
###########################   application  ####################################
###############################################################################

SET(NESBOY src/CPU.cpp src/Mapper.cpp src/MapperSxROM.cpp src/Cartridge.cpp src/MapperAxROM.cpp src/MapperUxROM.cpp src/Controller.cpp src/MapperCNROM.cpp src/PPU.cpp src/Emulator.cpp	src/MapperColorDreams.cpp src/PictureBus.cpp src/KeybindingsParser.cpp src/MapperGxROM.cpp src/VirtualScreen.cpp src/Log.cpp src/MapperMMC3.cpp src/main.cpp src/MainBus.cpp src/MapperNROM.cpp)
ADD_EXECUTABLE(nesboy ${NESBOY})
TARGET_LINK_LIBRARIES(nesboy sfml-system sfml-audio sfml-network sfml-graphics sfml-window)









