include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${QT_INCLUDE_DIR})

flex_target(analyzer analyzer.l ${CMAKE_CURRENT_BINARY_DIR}/analyzer.cpp)
bison_target(parser parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp COMPILE_FLAGS "--defines")
add_flex_bison_dependency(analyzer parser)

if (MSVC)
    find_path(FLEX_UNISTD_H_DIR unistd.h)
    if (FLEX_UNISTD_H_DIR)
        include_directories(${FLEX_UNISTD_H_DIR})
    endif()
endif()

add_executable(codegen ${FLEX_analyzer_OUTPUTS} ${BISON_parser_OUTPUTS} generator.cpp)
target_link_libraries(codegen ${QT_QTCORE_LIBRARY})
