#pragma once

#include "CallbackInterface.hpp"

void GLUTBackendInit( int argc, char** argv );

bool GLUTBackendCreateWindow( unsigned width, unsigned height, unsigned bpp, bool isFullScreen, const char* title );

void GLUTBackendRun( CallbackInterface* callbacks );
