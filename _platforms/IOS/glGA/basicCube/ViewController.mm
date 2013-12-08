
#import "ViewController.h"
#include "PlatformWrapper.h"
#include "glGAHelper.h"
#include <glm/glm.hpp>

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


@interface ViewController ()

@end

@implementation ViewController

- (void)viewDidLoad{
    [super viewDidLoad];
    
    EAGLContext* context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    [EAGLContext setCurrentContext:context];
    
    //create View
    //CGRect screen_bounds = [[UIScreen mainScreen] bounds];
    //GLKView* gl_view = [[GLKView alloc] initWithFrame:screen_bounds context:context];
    GLKView *gl_view = (GLKView *)self.view;
    gl_view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
    gl_view.context = context;
    
    glDisable(GL_CULL_FACE);
    [self initCube];
    
}

-(void) update{
    //Logic
}

-(void) glkView:(GLKView *)view drawInRect:(CGRect)rect {
    glClear( GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT );
    glClearColor( 0.0, 0.0, 0.0, 1.0);

    [self displayCube];
}

- (void)didReceiveMemoryWarning{
    [super didReceiveMemoryWarning];
}

void quad( int a, int b, int c, int d ){
	colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	colors[Index] = vertex_colors[b]; points[Index] = vertices[b]; Index++;
	colors[Index] = vertex_colors[c]; points[Index] = vertices[c]; Index++;
	colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	colors[Index] = vertex_colors[c]; points[Index] = vertices[c]; Index++;
	colors[Index] = vertex_colors[d]; points[Index] = vertices[d]; Index++;
}

void colorcube(){
	quad( 1, 0, 3, 2 );
	quad( 2, 3, 7, 6 );
	quad( 3, 0, 4, 7 );
	quad( 6, 5, 1, 2 );
	quad( 4, 5, 6, 7 );
	quad( 5, 4, 0, 1 );
}

-(void) initCube{
    
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

-(void) displayCube {
    glUseProgram(program);
	PLATFORM_glBindVertexArray(vao);

	glDrawArrays( GL_TRIANGLES, 0, NumVertices );
    
	PLATFORM_glBindVertexArray(0);

}

@end
