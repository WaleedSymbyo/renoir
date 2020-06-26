cmake_minimum_required(VERSION 3.16)

# list the header files
set(HEADER_FILES
	include/renoir-window/Window.h
)

# list the source files
set(SOURCE_FILES
)

# list os specfic files
if(WIN32)
	set(SOURCE_FILES ${SOURCE_FILES}
		src/renoir-window/winos/Window.cpp
	)
elseif(UNIX AND NOT APPLE)
	set(SOURCE_FILES ${SOURCE_FILES}
		src/renoir-window/linux/Window.cpp
	)
elseif(APPLE)
	set(SOURCE_FILES ${SOURCE_FILES}
		
	)
endif()

# add library target
add_library(renoir-window)

target_sources(renoir-window
	PRIVATE ${HEADER_FILES}
			${SOURCE_FILES}
)

if (RENOIR_UNITY_BUILD)
	set_target_properties(renoir-window
		PROPERTIES UNITY_BUILD_BATCH_SIZE 0
				   UNITY_BUILD true)
endif()

add_library(Renoir::renoir-window ALIAS renoir-window)

# make it reflect the same structure as the one on disk
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${HEADER_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_FILES})

# enable C++17
# disable any compiler specifc extensions
# add d suffix in debug mode
target_compile_features(renoir-window PUBLIC cxx_std_17)
set_target_properties(renoir-window PROPERTIES
	CXX_EXTENSIONS OFF
	DEBUG_POSTFIX d
)

# generate exports header file
include(GenerateExportHeader)
generate_export_header(renoir-window
	EXPORT_FILE_NAME ${CMAKE_CURRENT_SOURCE_DIR}/include/renoir-window/Exports.h
)

# list include directories
target_include_directories(renoir-window
	PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include
)
