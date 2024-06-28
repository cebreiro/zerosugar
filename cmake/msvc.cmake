add_compile_options(
    /W4
    /WX
    /MP
)

add_compile_definitions(
	NOMINMAX
	WIN32_LEAN_AND_MEAN
	_WIN32_WINNT=0x0A00
)

if (CMAKE_CXX_FLAGS MATCHES "-fsanitize=address")
    add_link_options(/INCREMENTAL:NO)

elseif (CMAKE_CONFIGURATION_TYPES STREQUAL RelWithDebInfo)
    add_compile_options(/wd4702)

elseif (POLICY CMP0141)
    # Enable Hot Reload for MSVC compilers if supported.
    cmake_policy(SET CMP0141 NEW)
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")

endif ()
