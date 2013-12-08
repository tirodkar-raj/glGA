//#version 120
//#version 150 core

attribute  vec4 vPosition;
attribute  vec4 vColor;
varying lowp vec4 color;

//in  vec4 vPosition;
//in  vec4 vColor;
//out vec4 color;


void main() 
{
  gl_Position = vPosition;
  color = vColor;
} 
