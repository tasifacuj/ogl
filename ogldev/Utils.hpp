#ifndef UTILS_HPP
#define UTILS_HPP

#define ARRAY_SIZE_IN_ELEMENTS( a ) ( sizeof( a ) / sizeof( a[ 0 ] ) )

#define INVALID_OGL_VALUE 0xFFFFFFFF

#define GLCheckError() (glGetError() == GL_NO_ERROR)


#define SAFE_DELETE(p) if (p) { delete p; p = NULL; }

#endif // UTILS_HPP
