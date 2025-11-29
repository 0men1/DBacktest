file(GLOB SOURCES CONFIGURE_DEPENDS "*.cpp")

add_library(src ${SOURCES})

target_include_directories(src PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}
)
