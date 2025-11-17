cmake_minimum_required(VERSION 3.25)

project(tinker
    DESCRIPTION "Tinker with asio networking"
)

include(FetchContent)

FetchContent_Declare(asio 
	GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
	GIT_TAG asio-1-36-0)
FetchContent_MakeAvailable(asio)

add_library(asio INTERFACE)
target_include_directories(asio SYSTEM INTERFACE ${asio_SOURCE_DIR}/include)

FetchContent_Declare(cx
    GIT_REPOSITORY https://github.com/sakchhu/cx.git
    GIT_TAG v0.1.0
)
FetchContent_MakeAvailable(cx)

function(add_snippet_target TARGET)
	add_executable(${TARGET} src/${TARGET}.cpp)
	set_target_properties(
		${TARGET}
		PROPERTIES
		CXX_STANDARD 23
	)
	target_link_libraries(${TARGET} asio cx)

	target_compile_options(${TARGET} 
		PRIVATE 
		-Wall
	)
endfunction()

add_snippet_target(tds)
add_snippet_target(tdc)

add_subdirectory(src)