#application

set(CORE_SRCS
    ${CORE_SRCS}
    ${CMAKE_CURRENT_SOURCE_DIR}/application.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/abstractlayer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/basicguilayer.cpp
    
    PARENT_SCOPE
)

set(CORE_HEADERS
    ${CORE_HEADERS}
    ${CMAKE_CURRENT_SOURCE_DIR}/application.h
    ${CMAKE_CURRENT_SOURCE_DIR}/abstractlayer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/basicguilayer.h
    
    PARENT_SCOPE
)
