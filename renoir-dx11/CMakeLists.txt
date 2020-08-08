cmake_minimum_required(VERSION 3.16)

# list the header files
set(HEADER_FILES
)

# list the source files
set(SOURCE_FILES
	src/renoir-dx11/Renoir-dx11.cpp
)

# add library target
add_library(renoir-dx11)

target_sources(renoir-dx11
	PRIVATE ${HEADER_FILES}
			${SOURCE_FILES}
			${PROJECT_SOURCE_DIR}/include/renoir/Renoir.h
)

set_target_properties(renoir-dx11 PROPERTIES PREFIX "")

if (RENOIR_UNITY_BUILD)
	set_target_properties(renoir-dx11
		PROPERTIES UNITY_BUILD_BATCH_SIZE 0
				   UNITY_BUILD true)
endif()

add_library(MoustaphaSaad::renoir-dx11 ALIAS renoir-dx11)

target_link_libraries(renoir-dx11
	PRIVATE
		mn
		d3d11
		dxgi
		d3dcompiler
		dxguid
)

# make it reflect the same structure as the one on disk
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${HEADER_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_FILES})

# enable C++17
# disable any compiler specifc extensions
target_compile_features(renoir-dx11 PUBLIC cxx_std_17)
set_target_properties(renoir-dx11 PROPERTIES
	CXX_EXTENSIONS OFF
)

# generate exports header file
include(GenerateExportHeader)
generate_export_header(renoir-dx11
	EXPORT_FILE_NAME ${CMAKE_CURRENT_SOURCE_DIR}/include/renoir-dx11/Exports.h
)

# list include directories
target_include_directories(renoir-dx11
	PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${PROJECT_SOURCE_DIR}/include
)
