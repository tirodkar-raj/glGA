
#import "ViewController.h"
#include <glm/glm.hpp>

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
    
    
    glm::vec4   origin(0.0f, 0.0f,0.0f,1.0f);
    // test a simple GLM matrix
    glm::mat4   matrix( 1.0, 0.0, 0.0, 0.0,
                       0.0, 1.0, 0.0, 0.0,
                       0.0, 0.0, 1.0, 0.0,
                       0.0, 0.0, 0.0, 1.0);
    
}

-(void) update{
    //Logic
}

-(void) glkView:(GLKView *)view drawInRect:(CGRect)rect {
    glClear( GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT );
    glClearColor( 0.0, 0.0, 1.0, 1.0);
}

- (void)didReceiveMemoryWarning{
    [super didReceiveMemoryWarning];
    
}

@end
