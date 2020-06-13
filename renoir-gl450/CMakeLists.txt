cmake_minimum_required(VERSION 3.16)

# list the header files
set(HEADER_FILES
	include/renoir-gl450/Context.h
	include/renoir-gl450/Handle.h
	include/renoir-gl450/Renoir-gl450.h
)

# list the source files
set(SOURCE_FILES
	src/renoir-gl450/Renoir-gl450.cpp
)

# list os specfic files
if(WIN32)
	set(SOURCE_FILES ${SOURCE_FILES}
		src/renoir-gl450/winos/Context.cpp
	)
elseif(UNIX AND NOT APPLE)
	set(SOURCE_FILES ${SOURCE_FILES}
	)
elseif(APPLE)
	set(SOURCE_FILES ${SOURCE_FILES}
	)
endif()

# add library target
add_library(renoir-gl450)

target_sources(renoir-gl450
	PRIVATE ${HEADER_FILES}
			${SOURCE_FILES}
			${PROJECT_SOURCE_DIR}/include/renoir/Renoir.h
)

if (RENOIR_UNITY_BUILD)
	set_target_properties(renoir-gl450
		PROPERTIES UNITY_BUILD_BATCH_SIZE 0
				   UNITY_BUILD true)
endif()

add_library(Renoir::renoir-gl450 ALIAS renoir-gl450)

target_link_libraries(renoir-gl450
	PRIVATE
		glew
		SPIRV
		glslang
		mn
		"$<$<PLATFORM_ID:Windows>:opengl32>"
)

# make it reflect the same structure as the one on disk
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${HEADER_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_FILES})

# enable C++17
# disable any compiler specifc extensions
# add d suffix in debug mode
target_compile_features(renoir-gl450 PUBLIC cxx_std_17)
set_target_properties(renoir-gl450 PROPERTIES
	CXX_EXTENSIONS OFF
	DEBUG_POSTFIX d
)

# generate exports header file
include(GenerateExportHeader)
generate_export_header(renoir-gl450
	EXPORT_FILE_NAME ${CMAKE_CURRENT_SOURCE_DIR}/include/renoir-gl450/Exports.h
)

# list include directories
target_include_directories(renoir-gl450
	PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${PROJECT_SOURCE_DIR}/include
)
