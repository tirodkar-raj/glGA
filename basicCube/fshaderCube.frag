//#version 120
//varying  vec4  color;

#version 150 core
in vec4 color;
out  vec4  colorOUT;

void main() 
{ 
    //gl_FragColor = color;
    colorOUT = color;
    //colorOUT = vec4(0.0,1.0,0.0,1.0);
} 

