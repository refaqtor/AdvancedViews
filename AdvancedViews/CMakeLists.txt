set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
find_package(Qt5Quick)
set(TRG_NAME AdvancedViews)
set(TRG_SOURCES
    advancedviews_plugin.cpp
    axis.cpp
    range.cpp
    tableviewprivate.cpp
)
set(TRG_HEADERS
    advancedviews_plugin.h
    axis.h
    cell.h
    range.h
    stdutils.h
    table.h
    tableviewprivate.h
)
set(TRG_RESOURCES
    resources.qrc
)
configure_file(qmldir ${CMAKE_CURRENT_BINARY_DIR})
add_library(${TRG_NAME} SHARED ${TRG_SOURCES} ${TRG_HEADERS} ${TRG_RESOURCES})
set_target_properties(${TRG_NAME} PROPERTIES CXX_STANDARD 17)
target_link_libraries(${TRG_NAME} Qt5::Quick)
