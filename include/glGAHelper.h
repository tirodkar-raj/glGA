//
//  glGAHelper.h
//
//
//  Created by George Papagiannakis.
//  Copyright (c) 2012 UoC & FORTH. All rights reserved.
//

#ifndef glGACharacterApp_glGAHelper_h
#define glGACharacterApp_glGAHelper_h

#include "PlatformWrapper.h"

#define USE_MAGICK
//#define _WINDOWS_TIME_
//#define _LINUX_TIME_

#ifdef USE_MAGICK
// Magick ++ lib from ImageMagick
// http://www.imagemagick.org/script/binary-releases.php
	#ifndef __IOS__
		#include <Magick++.h>
	#endif

#endif

#include <stdlib.h>
#include <stdio.h>
//#include <string.h>

///*
#ifndef __APPLE__  // include Mac OS X verions of headers
#  include <GL/glew.h>
#endif  // __APPLE__
//*/

// Define a helpful macro for handling offsets into buffer objects
#define BUFFER_OFFSET( offset )   ((GLvoid*) (offset))

#define ZERO_MEM(a) memset(a, 0, sizeof(a))

#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

#define INVALID_OGL_VALUE 0xFFFFFFFF

#define SAFE_DELETE(p) if (p) { delete p; p = NULL; }

#define GLExitIfError()                                                          \
{                                                                               \
GLenum Error = glGetError();                                                \
\
if (Error != GL_NO_ERROR) {                                                 \
printf("OpenGL error in %s:%d: 0x%x\n", __FILE__, __LINE__, Error);     \
}                                                                           \
}

#define GLCheckError() (glGetError() == GL_NO_ERROR)

// --------------------- SHADER helper methods ------------------/
//dual method in C
static char*readShaderSource(const char* shaderFile);
GLuint InitShader(const char* vShaderFile, const char* fShaderFile);
// alternative single C++ method
GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path);


#ifdef USE_MAGICK
// ------------ TEXTURE helper class (specified in .cpp)--------/
class Texture
{
public:
    Texture(GLenum textureTarget, const std::string& fileName);
    
    bool loadTexture();
    
    void bindTexture(GLenum textureUnit);
    
private:
    std::string     m_fileName;
    GLenum          m_textureTarget;
    GLuint          m_textureObj;
    Magick::Image*  m_pImage;
    Magick::Blob    m_blob;
    
};
#endif


#ifdef _WINDOWS_TIME_
#include < time.h >
#include <windows.h> //I've ommited this line.

#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
	#define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
#else
	#define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
#endif

struct timezone
{
    int  tz_minuteswest; /* minutes W of Greenwich */
    int  tz_dsttime;     /* type of dst correction */
};

int gettimeofday(struct timeval *tv, struct timezone *tz);
/*
 Usage
 Code Block
 gettimeofday(&now, NULL);
 gettimeofday(&now, &tzone);
 */
#endif

#ifdef _LINUX_TIME_
#include <sys/time.h>
#endif


#endif
