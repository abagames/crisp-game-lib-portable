cmake_minimum_required(VERSION 3.14)
set(CMAKE_C_STANDARD 11)

set(ENVSDK $ENV{PLAYDATE_SDK_PATH})

if (NOT ${ENVSDK} STREQUAL "")
	# Convert path from Windows
	file(TO_CMAKE_PATH ${ENVSDK} SDK)
else()
	execute_process(
			COMMAND bash -c "egrep '^\\s*SDKRoot' $HOME/.Playdate/config"
			COMMAND head -n 1
			COMMAND cut -c9-
			OUTPUT_VARIABLE SDK
			OUTPUT_STRIP_TRAILING_WHITESPACE
	)
endif()

if (NOT EXISTS ${SDK})
	message(FATAL_ERROR "SDK Path not found; set ENV value PLAYDATE_SDK_PATH")
	return()
endif()

set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
set(CMAKE_XCODE_GENERATE_SCHEME TRUE)

# Game Name Customization
set(PLAYDATE_GAME_NAME crisp-game-lib-portable)
set(PLAYDATE_GAME_DEVICE crisp-game-lib-portable-DEVICE)

project(${PLAYDATE_GAME_NAME} C ASM)

file(GLOB LibSources ../lib/*.c ../lib/*.h)
file(GLOB GamesSources ../games/*.c)

if (TOOLCHAIN STREQUAL "armgcc")
	add_compile_options(-D PLAYDATE_ARM_DEVICE)
	add_executable(${PLAYDATE_GAME_DEVICE} ${SDK}/C_API/buildsupport/setup.c ./src/cglpPlaydate.c ${LibSources} ${GamesSources})
	target_include_directories(${PLAYDATE_GAME_DEVICE} PRIVATE ../lib)
else()
	add_library(${PLAYDATE_GAME_NAME} SHARED ./src/cglpPlaydate.c ${LibSources} ${GamesSources})
	target_include_directories(${PLAYDATE_GAME_NAME} PRIVATE ../lib)
endif()

include(${SDK}/C_API/buildsupport/playdate_game.cmake)
