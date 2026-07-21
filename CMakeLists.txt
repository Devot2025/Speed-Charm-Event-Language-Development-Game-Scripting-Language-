cmake_minimum_required(VERSION 3.13)
project(speed_charm_event C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)


add_executable(
	"speed_charm_event"
	"app_1.c"
	"sce_lexer.c"
	"sce_parser.c"
	"sce_ir.c"
	"sce_run.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/std_ext/ext_alloc.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/std_ext/ext_strs.c"
)


target_include_directories(speed_charm_event
	PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}"
	"${CMAKE_CURRENT_SOURCE_DIR}/std_ext"
)