TEMPLATE = app
CONFIG += console c++1z
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    Pipeline.cpp \
    math_3d.cpp \
    Camera.cpp \
    Texture.cpp \
    image_loader/stb_image.cpp \
    GLUTBackend.cpp \
    Technique.cpp \
    LightTechnique.cpp \
    Mesh.cpp

HEADERS += \
    math_3d.h \
    Pipeline.hpp \
    Camera.hpp \
    Texture.hpp \
    image_loader/stb_image.h \
    GlutBackEnd.hpp \
    CallbackInterface.hpp \
    Technique.hpp \
    LightTechnique.hpp \
    Utils.hpp \
    Mesh.hpp



LIBS += -lglut -lGL -lGLEW -lassimp -lMagick++-6.Q16 -lMagickCore-6.Q16

INCLUDEPATH += /usr/include/ImageMagick-6  /usr/include/ImageMagick-6/Magick++ /usr/include/x86_64-linux-gnu/ImageMagick-6/

