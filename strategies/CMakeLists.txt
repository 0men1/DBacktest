file(GLOB SOURCES CONFIGURE_DEPENDS "*.cpp")

add_library(strategies ${SOURCES})

target_include_directories(strategies PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}
)
