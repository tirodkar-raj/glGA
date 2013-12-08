
#import "ViewController.h"
#include "PlatformWrapper.h"
#include "glGAHelper.h"
#include <glm/glm.hpp>

GLuint programTriangle;
GLuint vaoTriangle;
GLuint bufferTriangle;

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
    [self initTriangle];
}

-(void) update{
    //Logic
}

-(void) glkView:(GLKView *)view drawInRect:(CGRect)rect {
    glClear( GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT );
    glClearColor( 0.0, 0.0, 0.0, 1.0); //black color
    
    [self displayTriangle];
}

- (void)didReceiveMemoryWarning{
    [super didReceiveMemoryWarning];
    
}




-(void) initTriangle {
	//generate and bind a VAO for the 3D axes
	PLATFORM_glGenVertexArrays(1, &vaoTriangle);
	PLATFORM_glBindVertexArray(vaoTriangle);
    
	//load shaders
	programTriangle = LoadShaders("vshaderSimple.vert", "fshaderSimple.frag");
	glUseProgram(programTriangle);
    
    
    
    //vertices for a 3D triangle
    /*      1
     *   *
     *       *
     0    *****    2
     
     */
    glm::vec4 triangleVertices[3]=
    {
        glm::vec4(0.0,0.0,0.0,1.0),
        glm::vec4(0.5,1.0,0.0,1.0),
        glm::vec4(1.0,0.0,0.0,1.0)
    };
    
	//create vertices VBO on server side GPU
	glGenBuffers(1,&bufferTriangle);
	glBindBuffer(GL_ARRAY_BUFFER, bufferTriangle);
	//fill-in VBO with vertices
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices),triangleVertices, GL_STATIC_DRAW);
	//connect vertex shader with application vertex data
	GLuint  vPosit = glGetAttribLocation(programTriangle, "vPosition");
	glEnableVertexAttribArray(vPosit);
	glVertexAttribPointer(vPosit,4, GL_FLOAT, GL_FALSE,0, BUFFER_OFFSET(0));
	//stop using previous VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    
	// only one VAO can be bound at a time, so disable it to avoid altering it accidentally
	PLATFORM_glBindVertexArray(0);
}

-(void) displayTriangle{
    
	glUseProgram(programTriangle);
	PLATFORM_glBindVertexArray(vaoTriangle);
    
	glDrawArrays( GL_TRIANGLES, 0, 3 );

	PLATFORM_glBindVertexArray(0);
    
}


@end
