//
//  main.cpp
//  basicCube
//
//  Created by George Papagiannakis on 23/10/12.
//  Copyright (c) 2012 University Of Crete & FORTH. All rights reserved.
//


// basic STL streams
#include <iostream>

#include "PlatformWrapper.h"


// GLM lib
// http://glm.g-truc.net/api/modules.html
#define GLM_SWIZZLE
#define GLM_FORCE_INLINE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>

#include "glGAHelper.h"

// global variables
int         windowWidth=1024, windowHeight=768;
GLuint      program;
GLuint      vao;
GLuint      buffer;
bool        wireFrame=false;
typedef     glm::vec4   color4;
typedef     glm::vec4   point4;
int                     Index = 0;
const       int         NumVertices = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)

// Modelling arrays
point4      points[NumVertices];
color4      colors[NumVertices];
glm::vec3   normals[NumVertices];

// API function prototypes
void GLFWCALL   WindowSizeCB(int, int);
void GLFWCALL   OnMouseButton(int glfwButton, int glfwAction);
void GLFWCALL   OnMousePos(int mouseX, int mouseY);
void GLFWCALL   OnMouseWheel(int pos);
void GLFWCALL   OnKey(int glfwKey, int glfwAction);
void GLFWCALL   OnChar(int glfwChar, int glfwAction);


// Vertices of a unit cube centered at origin, sides aligned with axes
point4 vertices[8] = {
	point4( -0.5, -0.5,  0.5, 1.0 ),
	point4( -0.5,  0.5,  0.5, 1.0 ),
	point4(  0.5,  0.5,  0.5, 1.0 ),
	point4(  0.5, -0.5,  0.5, 1.0 ),
	point4( -0.5, -0.5, -0.5, 1.0 ),
	point4( -0.5,  0.5, -0.5, 1.0 ),
	point4(  0.5,  0.5, -0.5, 1.0 ),
	point4(  0.5, -0.5, -0.5, 1.0 )
};

// RGBA olors
color4 vertex_colors[8] = {
	color4( 0.0, 0.0, 0.0, 1.0 ),  // black
	color4( 1.0, 0.0, 0.0, 1.0 ),  // red
	color4( 1.0, 1.0, 0.0, 1.0 ),  // yellow
	color4( 0.0, 1.0, 0.0, 1.0 ),  // green
	color4( 0.0, 0.0, 1.0, 1.0 ),  // blue
	color4( 1.0, 0.0, 1.0, 1.0 ),  // magenta
	color4( 1.0, 1.0, 1.0, 1.0 ),  // white
	color4( 0.0, 1.0, 1.0, 1.0 )   // cyan
};




// Our function prototypes
void            initCube();
void            displayCube();
void            colorcube();
void            quad( int a, int b, int c, int d );

// quad generates two triangles for each face and assigns colors
//    to the vertices
void quad( int a, int b, int c, int d )
{
	colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	colors[Index] = vertex_colors[b]; points[Index] = vertices[b]; Index++;
	colors[Index] = vertex_colors[c]; points[Index] = vertices[c]; Index++;
	colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	colors[Index] = vertex_colors[c]; points[Index] = vertices[c]; Index++;
	colors[Index] = vertex_colors[d]; points[Index] = vertices[d]; Index++;
}

// generate 12 triangles: 36 vertices and 36 colors
void colorcube()
{
	quad( 1, 0, 3, 2 );
	quad( 2, 3, 7, 6 );
	quad( 3, 0, 4, 7 );
	quad( 6, 5, 1, 2 );
	quad( 4, 5, 6, 7 );
	quad( 5, 4, 0, 1 );
}



void initCube()
{
	//generate and bind a VAO for the 3D axes
	PLATFORM_glGenVertexArrays(1, &vao);
	PLATFORM_glBindVertexArray(vao);


	colorcube();

	// Load shaders and use the resulting shader program
	program = LoadShaders( "vshaderCube.vert", "fshaderCube.frag" );
	glUseProgram( program );

	// Create and initialize a buffer object on the server side (GPU)
	//GLuint buffer;
	glGenBuffers( 1, &buffer );
	glBindBuffer( GL_ARRAY_BUFFER, buffer );
	glBufferData( GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors),NULL, GL_STATIC_DRAW );
	glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(points), points );
	glBufferSubData( GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors );

	// set up vertex arrays
	GLuint vPosition = glGetAttribLocation( program, "vPosition" );
	glEnableVertexAttribArray( vPosition );
	glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0,BUFFER_OFFSET(0) );

	GLuint vColor = glGetAttribLocation( program, "vColor" ); 
	glEnableVertexAttribArray( vColor );
	glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0,BUFFER_OFFSET(sizeof(points)) );

	glEnable( GL_DEPTH_TEST );
	glClearColor( 0.0, 0.0, 0.0, 1.0 ); 


	// only one VAO can be bound at a time, so disable it to avoid altering it accidentally
	PLATFORM_glBindVertexArray(0);
}

void displayCube()
{
	glUseProgram(program);
	PLATFORM_glBindVertexArray(vao);
	glDisable(GL_CULL_FACE);
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	if (wireFrame)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glDrawArrays( GL_TRIANGLES, 0, NumVertices );

	glPopAttrib();
	PLATFORM_glBindVertexArray(0);
}


// Callback function called by GLFW when window size changes
void GLFWCALL WindowSizeCB(int width, int height)
{
    // Set OpenGL viewport and default camera
    glViewport(0, 0, width, height);
}


// Callback function called by GLFW when a mouse button is clicked
void GLFWCALL OnMouseButton(int glfwButton, int glfwAction)
{
    // Do something if needed.
}


// Callback function called by GLFW when mouse has moved
void GLFWCALL OnMousePos(int mouseX, int mouseY)
{
    // Do something if needed.
}


// Callback function called by GLFW on mouse wheel event
void GLFWCALL OnMouseWheel(int pos)
{
    // Do something if needed.
}


// Callback function called by GLFW on key event
void GLFWCALL OnKey(int glfwKey, int glfwAction)
{
    
    if( glfwKey==GLFW_KEY_ESC && glfwAction==GLFW_PRESS ) // Want to quit?
        glfwCloseWindow();
    else if( glfwKey=='W' && glfwAction==GLFW_PRESS )
    {
        std::cout<<" 'W' key was pressed! "<<std::endl;
        if (wireFrame==true) {
            wireFrame =false;
        }
        else
            wireFrame=true;
    }
    else
    {
        // Event has not been handled
        // Do something if needed.
    }
    
}


// Callback function called by GLFW on char event
void GLFWCALL OnChar(int glfwChar, int glfwAction)
{
    // Do something if needed.
}



int main (int argc, const char * argv[])
{
    // test a simple GLM vector
    glm::vec4   origin(0.0f, 0.0f,0.0f,1.0f);
    // test a simple GLM matrix
    glm::mat4   mat( 1.0, 0.0, 0.0, 0.0,
                    0.0, 1.0, 0.0, 0.0,
                    0.0, 0.0, 1.0, 0.0,
                    0.0, 0.0, 0.0, 1.0);
    
    // initialise GLFW
    int running = GL_TRUE;
    
    if (!glfwInit()) {
        exit(EXIT_FAILURE);
    }
    
    //only for OpenGL 2.1
#ifdef USE_OPENGL21
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 2);
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 1);
#endif
    
    //Only for OpenGL 3.2
#ifdef USE_OPENGL32
    glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 4);
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 2);
    glfwOpenWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwOpenWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);
#endif
    
    GLFWvidmode mode;
    glfwGetDesktopMode(&mode);
    if( !glfwOpenWindow(windowWidth, windowHeight, mode.RedBits, mode.GreenBits, mode.BlueBits, 0, 32, 0, GLFW_WINDOW /* or GLFW_FULLSCREEN */) )
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwEnable(GLFW_MOUSE_CURSOR);
    glfwEnable(GLFW_KEY_REPEAT);
    // Ensure we can capture the escape key being pressed below
	glfwEnable( GLFW_STICKY_KEYS );
	glfwSetMousePos(windowWidth/2, windowHeight/2);
    glfwSetWindowTitle("basic Cube HY358");
    
    //init GLEW and basic OpenGL information
    // VERY IMPORTANT OTHERWISE GLEW CANNOT HANDLE GL3
#ifdef USE_OPENGL32
    glewExperimental = true; 
#endif
    glewInit();
    std::cout<<"\nUsing GLEW "<<glewGetString(GLEW_VERSION)<<std::endl;
    if (GLEW_VERSION_2_1)
    {
        std::cout<<"\nYay! OpenGL 2.1 is supported and GLSL 1.2!\n"<<std::endl;
    }
    if (GLEW_VERSION_3_2)
    {
        std::cout<<"Yay! OpenGL 3.2 is supported and GLSL 1.5!\n"<<std::endl;
    }
    
    /*
     This extension defines an interface that allows various types of data
     (especially vertex array data) to be cached in high-performance
     graphics memory on the server, thereby increasing the rate of data
     transfers.
     Chunks of data are encapsulated within "buffer objects", which
     conceptually are nothing more than arrays of bytes, just like any
     chunk of memory.  An API is provided whereby applications can read
     from or write to buffers, either via the GL itself (glBufferData,
     glBufferSubData, glGetBufferSubData) or via a pointer to the memory.
     */
	if (glewIsSupported("GL_ARB_vertex_buffer_object"))
		std::cout<<"ARB VBO's are supported"<<std::endl;
    else if (glewIsSupported("GL_APPLE_vertex_buffer_object"))
		std::cout<<"APPLE VBO's are supported"<<std::endl;
	else
		std::cout<<"VBO's are not supported,program will not run!!!"<<std::endl; 
    
    /* 
     This extension introduces named vertex array objects which encapsulate
     vertex array state on the client side. The main purpose of these 
     objects is to keep pointers to static vertex data and provide a name 
     for different sets of static vertex data.  
     By extending vertex array range functionality this extension allows multiple
     vertex array ranges to exist at one time, including their complete sets of
     state, in manner analogous to texture objects. 
     GenVertexArraysAPPLE creates a list of n number of vertex array object
     names.  After creating a name, BindVertexArrayAPPLE associates the name with
     a vertex array object and selects this vertex array and its associated
     state as current.  To get back to the default vertex array and its
     associated state the client should bind to vertex array named 0.
     */
    
	if (glewIsSupported("GL_ARB_vertex_array_object"))
        std::cout<<"ARB VAO's are supported\n"<<std::endl;
    else if (glewIsSupported("GL_APPLE_vertex_array_object"))//this is the name of the extension for GL2.1 in MacOSX
		std::cout<<"APPLE VAO's are supported\n"<<std::endl;
	else
		std::cout<<"VAO's are not supported, program will not run!!!\n"<<std::endl;
    
    
    std::cout<<"Vendor: "<<glGetString (GL_VENDOR)<<std::endl;
    std::cout<<"Renderer: "<<glGetString (GL_RENDERER)<<std::endl;
    std::cout<<"Version: "<<glGetString (GL_VERSION)<<std::endl;
    
    // Set GLFW event callbacks
    // - Redirect window size changes to the callback function WindowSizeCB
    glfwSetWindowSizeCallback(WindowSizeCB);
    
    // - Directly redirect GLFW mouse button events to AntTweakBar
    //glfwSetMouseButtonCallback((GLFWmousebuttonfun)TwEventMouseButtonGLFW);
    glfwSetMouseButtonCallback(OnMouseButton);
    
    // - Directly redirect GLFW mouse position events to AntTweakBar
    //glfwSetMousePosCallback((GLFWmouseposfun)TwEventMousePosGLFW);
    glfwSetMousePosCallback(OnMousePos);
    
    // - Directly redirect GLFW mouse wheel events to AntTweakBar
    //glfwSetMouseWheelCallback((GLFWmousewheelfun)TwEventMouseWheelGLFW);
    glfwSetMouseWheelCallback(OnMouseWheel);
    
    // - Directly redirect GLFW key events to AntTweakBar
    //glfwSetKeyCallback((GLFWkeyfun)TwEventKeyGLFW);
    glfwSetKeyCallback(OnKey);
    
    // - Directly redirect GLFW char events to AntTweakBar
    //glfwSetCharCallback((GLFWcharfun)TwEventCharGLFW);
    glfwSetCharCallback(OnChar);
    
    // Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);
    
    // init Scene
    initCube();
    
    
    //GLFW main loop
    while (running) {
		glClear( GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT );
		glClearColor( 0.0, 0.0, 0.0, 1.0);
		// call function to render our scene
		displayCube();
        
        glfwSwapBuffers();
        //check if ESC was pressed
        running=!glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED);
    }
    
    
    //close OpenGL window and terminate GLFW
    glfwTerminate();
	    
    std::cout << "Hello, GLFW, GLEW, GLM Graphics World!\n";
    
    exit(EXIT_SUCCESS);
    
}



