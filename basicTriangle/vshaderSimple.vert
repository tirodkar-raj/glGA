//#version 120 // in GL2.1
//attribute  vec4 vPosition; // in GL2.1

#version 150 core //in GL3.2
in  vec4 vPosition; // in GL3.2

void main() 
{
  gl_Position = vPosition;
} 
