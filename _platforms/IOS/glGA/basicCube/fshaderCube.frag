//#version 120
//#version 150 core

varying lowp vec4  color;
//in vec4 color;
//out  vec4  colorOUT;

void main() 
{ 
    gl_FragColor = color;
    //colorOUT = color;
    //colorOUT = vec4(0.0,1.0,0.0,1.0);
} 

