include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

flex_target(analyzer analyzer.l ${CMAKE_CURRENT_BINARY_DIR}/analyzer.cpp)
bison_target(parser parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp COMPILE_FLAGS "--defines")
add_flex_bison_dependency(analyzer parser)

add_executable(codegen ${FLEX_analyzer_OUTPUTS} ${BISON_parser_OUTPUTS} generator.cpp)
target_link_libraries(codegen ${QT_QTCORE_LIBRARY})
