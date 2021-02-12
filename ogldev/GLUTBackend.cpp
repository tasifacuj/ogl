#include "GlutBackEnd.hpp"

#include <unistd.h>
#include <cstdio>
#include <GL/glew.h>
#include <GL/freeglut.h>

static CallbackInterface* s_pCallbacks = nullptr;

static void special_keyboard_cb( int key, int x, int y ){
    s_pCallbacks->specialKeyboardCB( key, x, y );
}

static void keyboard_cb( unsigned char key, int x, int y ){
    s_pCallbacks->keyboardCB( key, x, y );
}

static void passive_mouse_cb( int x, int y ){
    s_pCallbacks->passiveMouseCB( x, y );
}

static void render_scene_cb(){
    s_pCallbacks->renderSceneCB();
}

static void idle_cb(){
    s_pCallbacks->idleCB();
}

static void init_callbacks(){
    glutDisplayFunc( render_scene_cb );
    glutIdleFunc( idle_cb );
    glutSpecialFunc( special_keyboard_cb );
    glutPassiveMotionFunc( passive_mouse_cb );
    glutKeyboardFunc( keyboard_cb );
}

void GLUTBackendInit(int argc, char **argv){
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );
    glutSetOption( GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS );
}

bool GLUTBackendCreateWindow(unsigned width, unsigned height, unsigned bpp, bool isFullScreen, const char *title){
    if( isFullScreen ){
        char mode_str[ 64 ] = { 0 };
        snprintf( mode_str, sizeof( mode_str ), "%dx%d@%d", width, height, bpp );
        glutGameModeString( mode_str );
        glutEnterGameMode();
    }else{
        glutInitWindowSize( width, height );
        glutCreateWindow( title );
    }

    GLenum res = glewInit();

    if( res != GLEW_OK ){
        fprintf( stderr, "Error: %s\n", glewGetErrorString( res ) );
        return false;
    }

    return true;
}

void GLUTBackendRun(CallbackInterface *callbacks){
    if( !callbacks ){
        fprintf( stderr, "%s - callbacks not specified\n", __FUNCTION__ );
        return;
    }

    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
    glEnable( GL_DEPTH_TEST );
//    glFrontFace( GL_CW );
//    glCullFace( GL_BACK );
//    glEnable( GL_CULL_FACE );
    s_pCallbacks = callbacks;
    init_callbacks();
    glutMainLoop();
}
