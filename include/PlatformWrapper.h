//
//  PlatformWrapper.h
//
//
//  Created by George Papagiannakis.
//  Copyright (c) 2013 UoC & FORTH. All rights reserved.
//

//#define __WINDOWS__MAC__LINUX__
#define __IOS__

#include <string>
using namespace std;


#ifdef __WINDOWS__MAC__LINUX__ 

// GLEW lib
// http://glew.sourceforge.net/basic.html
#include <GL/glew.h>

// Here we decide which of the two versions we want to use
// If your systems supports both, choose to uncomment USE_OPENGL32
// otherwise choose to uncomment USE_OPENGL21
// GLView cna also help you decide before running this program:
// 
//#define USE_OPENGL21
#define USE_OPENGL32

// GLFW lib
// http://www.glfw.org/documentation.html
#ifdef USE_OPENGL32
#define GLFW_INCLUDE_GL3
#define USE_GL3
#define GLFW_NO_GLU
#define GL3_PROTOTYPES 1 
#endif

#define GLFW_DLL //use GLFW as a dynamically linked library
#include <GL/glfw.h>

#define PLATFORM_glGenVertexArrays(x,y) glGenVertexArrays(x,y)
#define PLATFORM_glBindVertexArray(x) glBindVertexArray(x)


#elif defined(__IOS__)


#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>


#define PLATFORM_glGenVertexArrays(x,y) glGenVertexArraysOES(x,y)
#define PLATFORM_glBindVertexArray(x) glBindVertexArrayOES(x)

string getFilenameWithoutExtension(const char* filename);
string getExtension(const char* filename);
string getExtensionWithDot(const char* filename);
void getRidOfString(string& full_string, string part_string);
void replaceURLSpaces(string& str);
string getBundlePath(const char* filename);

#endif


string getTextureResourcePath(const char* filename);
string getGLSLResourcePath(const char* filename);
string getModelResourcePath(const char* filename);


