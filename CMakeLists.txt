cmake_minimum_required(VERSION 3.10)

option(GENERATE_HEADER "Generate single include header" ON)

project(Argless)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if (GENERATE_HEADER)
	file(GLOB_RECURSE SOURCES "src/Argless/*.hpp")

	set(MAIN_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/src/Argless.hpp")

	set(OUT_DIR "${CMAKE_SOURCE_DIR}/include")
	set(HEADER "${OUT_DIR}/Argless.hpp")

	set(HEADER_TEMP "${CMAKE_BINARY_DIR}/Argless.hpp.temp")

	set(LICENSE "${CMAKE_SOURCE_DIR}/LICENSE.md")

	set(HELLO "/* Argless - https://github.com/Maslowian/Argless */")

	find_program(QUOM "quom")
	if (NOT QUOM)
		message(FATAL_ERROR "quom not found")
	endif()

	# {HELLO} \n {LICENSE} \n {HEADER_TEMP} 
	add_custom_command(
		OUTPUT ${HEADER}
		COMMAND ${QUOM} ${MAIN_HEADER} ${HEADER_TEMP}
		COMMAND ${CMAKE_COMMAND} -E echo "${HELLO}" > ${HEADER}
		COMMAND ${CMAKE_COMMAND} -E echo "" >> ${HEADER}
		COMMAND ${CMAKE_COMMAND} -E echo "/*" >> ${HEADER}
		COMMAND ${CMAKE_COMMAND} -E cat ${LICENSE} >> ${HEADER}
		COMMAND ${CMAKE_COMMAND} -E echo "*/" >> ${HEADER}
		COMMAND ${CMAKE_COMMAND} -E echo "" >> ${HEADER}
		COMMAND ${CMAKE_COMMAND} -E cat ${HEADER_TEMP} >> ${HEADER}
		DEPENDS ${SOURCES}
	)

	add_custom_target(generate_header ALL DEPENDS ${HEADER})
endif()

include_directories("src/Argless")

add_library(${PROJECT_NAME} INTERFACE)

target_include_directories(${PROJECT_NAME} INTERFACE "src/")


add_subdirectory("Sandbox")
