INCLUDE_DIRECTORIES(
${BULLET_PHYSICS_SOURCE_DIR}/src
)

LINK_LIBRARIES(
 BulletDynamics BulletCollision LinearMath
)

IF (WIN32)
    ADD_EXECUTABLE(App_FaExample
        FaExample.cpp
        main.cpp
        ${BULLET_PHYSICS_SOURCE_DIR}/build3/bullet.rc
    )
ELSE()
    ADD_EXECUTABLE(App_FaExample
        FaExample.cpp
        main.cpp
    )
ENDIF()




IF (INTERNAL_ADD_POSTFIX_EXECUTABLE_NAMES)
            SET_TARGET_PROPERTIES(App_FaExample PROPERTIES  DEBUG_POSTFIX "_Debug")
            SET_TARGET_PROPERTIES(App_FaExample PROPERTIES  MINSIZEREL_POSTFIX "_MinsizeRel")
            SET_TARGET_PROPERTIES(App_FaExample PROPERTIES  RELWITHDEBINFO_POSTFIX "_RelWithDebugInfo")
ENDIF(INTERNAL_ADD_POSTFIX_EXECUTABLE_NAMES)





#################
# Standalone BasicExampleGui using OpenGL (but not the example browser)


INCLUDE_DIRECTORIES(
        ${BULLET_PHYSICS_SOURCE_DIR}/src
        ${BULLET_PHYSICS_SOURCE_DIR}/btgui
        ${BULLET_PHYSICS_SOURCE_DIR}/examples
        ${BULLET_PHYSICS_SOURCE_DIR}/examples/ThirdPartyLibs/Glew
)


SET(AppFaExampleGui_SRCS
    FaExample.cpp
        ${BULLET_PHYSICS_SOURCE_DIR}/build3/bullet.rc
    ../StandaloneMain/main_opengl_single_example.cpp
        ../ExampleBrowser/OpenGLGuiHelper.cpp
        ../ExampleBrowser/GL_ShapeDrawer.cpp
        ../ExampleBrowser/CollisionShape2TriangleMesh.cpp
        ../Utils/b3Clock.cpp
)

#this define maps StandaloneExampleCreateFunc to the right 'CreateFunc'
ADD_DEFINITIONS(-DB3_USE_STANDALONE_EXAMPLE)

LINK_LIBRARIES(
        BulletDynamics BulletCollision LinearMath OpenGLWindow Bullet3Common 
)

#some code to support OpenGL and Glew cross platform
IF (WIN32)
        INCLUDE_DIRECTORIES(
             ${BULLET_PHYSICS_SOURCE_DIR}/btgui/OpenGLWindow/GlewWindows
        )
        ADD_DEFINITIONS(-DGLEW_STATIC)
    LINK_LIBRARIES( ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY} )
ELSE(WIN32)
        IF(APPLE)
                find_library(COCOA NAMES Cocoa)
                MESSAGE(${COCOA})
                link_libraries(${COCOA} ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY})

        ELSE(APPLE)
                INCLUDE_DIRECTORIES(
                        ${BULLET_PHYSICS_SOURCE_DIR}/btgui/OpenGLWindow/GlewWindows
                )
                ADD_DEFINITIONS("-DGLEW_INIT_OPENGL11_FUNCTIONS=1")
                ADD_DEFINITIONS("-DGLEW_STATIC")
                ADD_DEFINITIONS("-DGLEW_DYNAMIC_LOAD_ALL_GLX_FUNCTIONS=1")

                LINK_LIBRARIES(  pthread ${DL} )
        ENDIF(APPLE)
ENDIF(WIN32)


ADD_EXECUTABLE(AppFaExampleGui
                ${AppFaExampleGui_SRCS}
)


IF (INTERNAL_ADD_POSTFIX_EXECUTABLE_NAMES)
                        SET_TARGET_PROPERTIES(AppFaExampleGui PROPERTIES  DEBUG_POSTFIX "_Debug")
                        SET_TARGET_PROPERTIES(AppFaExampleGui PROPERTIES  MINSIZEREL_POSTFIX "_MinsizeRel")
                        SET_TARGET_PROPERTIES(AppFaExampleGui PROPERTIES  RELWITHDEBINFO_POSTFIX "_RelWithDebugInfo")
ENDIF(INTERNAL_ADD_POSTFIX_EXECUTABLE_NAMES)


