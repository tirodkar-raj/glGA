
//#version 120
//attribute  vec4 vPosition;
//attribute  vec4 vColor;
//varying vec4 color;

#version 150 core
in  vec4 vPosition;
in  vec4 vColor;
out vec4 color;


void main() 
{
  gl_Position = vPosition;
  color = vColor;
} 
