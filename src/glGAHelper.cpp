//
//  glGAHelper.cpp
//
//
//  Created by George Papagiannakis.
//  Copyright (c) 2012 UoC & FORTH. All rights reserved.
//

#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <stdlib.h>
#include <string.h>

//#include <GL/glew.h>

#include "glGAHelper.h"

#include "PlatformWrapper.h"

#if defined(__APPLE__) && !defined(__IOS__)
#include <OpenGL/gl3.h>
#endif




//*********************************** SHADER Helper functions ***********************/

// Create a NULL-terminated string by reading the provided file (in C)
static char* readShaderSource(const char* shaderFile)
{
	FILE* fp = fopen(shaderFile, "r");

	if ( fp == NULL ) { return NULL; }

	fseek(fp, 0L, SEEK_END);
	long size = ftell(fp);

	fseek(fp, 0L, SEEK_SET);
	char* buf = new char[size + 1];
	fread(buf, 1, size, fp);

	buf[size] = '\0';
	fclose(fp);

	return buf;
}

// Create a GLSL program object from vertex and fragment shader files (in C)
GLuint InitShader(const char* vShaderFile, const char* fShaderFile)
{
	struct Shader {
		const char*  filename;
		GLenum       type;
		GLchar*      source;
	}  shaders[2] = {
		{ vShaderFile, GL_VERTEX_SHADER, NULL },
		{ fShaderFile, GL_FRAGMENT_SHADER, NULL }
	};

	GLuint program = glCreateProgram();

	for ( int i = 0; i < 2; ++i ) {
		Shader& s = shaders[i];
		s.source = readShaderSource( s.filename );
		if ( shaders[i].source == NULL ) {
			std::cerr << "Failed to read " << s.filename << std::endl;
			exit( EXIT_FAILURE );
		}

		GLuint shader = glCreateShader( s.type );
		glShaderSource( shader, 1, (const GLchar**) &s.source, NULL );
		glCompileShader( shader );

		GLint  compiled;
		glGetShaderiv( shader, GL_COMPILE_STATUS, &compiled );
		if ( !compiled ) {
			std::cerr << s.filename << " failed to compile:" << std::endl;
			GLint  logSize;
			glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &logSize );
			char* logMsg = new char[logSize];
			glGetShaderInfoLog( shader, logSize, NULL, logMsg );
			std::cerr << logMsg << std::endl;
			delete [] logMsg;

			exit( EXIT_FAILURE );
		}

		delete [] s.source;

		glAttachShader( program, shader );
	}

	/* link  and error check */
	glLinkProgram(program);

	GLint  linked;
	glGetProgramiv( program, GL_LINK_STATUS, &linked );
	if ( !linked ) {
		std::cerr << "Shader program failed to link" << std::endl;
		GLint  logSize;
		glGetProgramiv( program, GL_INFO_LOG_LENGTH, &logSize);
		char* logMsg = new char[logSize];
		glGetProgramInfoLog( program, logSize, NULL, logMsg );
		std::cerr << logMsg << std::endl;
		delete [] logMsg;

		exit( EXIT_FAILURE );
	}

	/* use program object */
	//   glUseProgram(program);

	return program;
}

// Alternative single C++ method to Load/Compile/Link a pair for vertex and fragment shaders
GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path)
{


	string str_vertex_filepath = getGLSLResourcePath(vertex_file_path);
	//cout << str_vertex_filepath << endl;
	string str_fragment_filepath = getGLSLResourcePath(fragment_file_path);
	//cout << str_fragment_filepath << endl;


	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(str_vertex_filepath.c_str(), std::ios::in);
	if(VertexShaderStream.is_open()){
		std::string Line = "";
		while(getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}
	else
	{
		std::cout<<"shader: "<<str_vertex_filepath<<" not found!"<<std::endl;
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(str_fragment_filepath.c_str(), std::ios::in);
	if(FragmentShaderStream.is_open()){
		std::string Line = "";
		while(getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}
	else
	{
		std::cout<<"shader: "<<str_fragment_filepath<<" not found!"<<std::endl;
		return 0;
	}

	GLint Result = GL_FALSE;
	int InfoLogLength=10;

	// Compile Vertex Shader
	printf("Compiling shader : %s\n\n", str_vertex_filepath.c_str());
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	if (!Result) 
	{
		glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		std::vector<char> VertexShaderErrorMessage(InfoLogLength);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);
	}
	// Compile Fragment Shader
	printf("Compiling shader : %s\n\n", str_fragment_filepath.c_str());
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	if (!Result) 
	{
		glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);
	}
	// Link the program
	fprintf(stdout, "Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> ProgramErrorMessage( max(InfoLogLength, int(1)) );
	glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
	fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);


	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}
//*********************************** SHADER Helper functions ***********************/


//*********************************** TEXTURE Helper functions ***********************/
#ifdef USE_MAGICK

Texture::Texture(GLenum textureTarget, const std::string& fileName)
{
	m_textureTarget = textureTarget;
	m_fileName      = fileName;
	m_pImage        = NULL;
	std::cout<< "Constructing texture is done with success! :"<<m_fileName<<std::endl;
}

bool Texture::loadTexture()
{
	try {
		m_pImage = new Magick::Image(m_fileName);
		m_pImage->write(&m_blob, "RGBA");
	} catch (Magick::Error& Error) {
		std::cout<< "Error loading texture: "<<m_fileName<<" :"<<Error.what() <<std::endl;
		return false;
	}
	//then if texture is loaded ok by ImageMagick
	glGenTextures(1, &m_textureObj);
	glBindTexture(m_textureTarget, m_textureObj);
	glTexImage2D(m_textureTarget, 0, GL_RGB, m_pImage->columns(), m_pImage->rows(), -0.5, GL_RGBA, GL_UNSIGNED_BYTE, m_blob.data());
	glTexParameterf(m_textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(m_textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	std::cout<< "loading texture is done with success! :"<<m_fileName<<std::endl;

	return true;
}

void Texture::bindTexture(GLenum textureUnit)
{
	glActiveTexture(textureUnit);
	glBindTexture(m_textureTarget, m_textureObj);
}


#endif


#ifdef _WINDOWS_TIME_
int gettimeofday(struct timeval *tv, struct timezone *tz)
{
	FILETIME ft;
	unsigned __int64 tmpres = 0;
	static int tzflag;

	if (NULL != tv)
	{
		GetSystemTimeAsFileTime(&ft);

		tmpres |= ft.dwHighDateTime;
		tmpres <<= 32;
		tmpres |= ft.dwLowDateTime;

		/*converting file time to unix epoch*/
		tmpres -= DELTA_EPOCH_IN_MICROSECS;
		tmpres /= 10;  /*convert into microseconds*/
		tv->tv_sec = (long)(tmpres / 1000000UL);
		tv->tv_usec = (long)(tmpres % 1000000UL);
	}

	if (NULL != tz)
	{
		if (!tzflag)
		{
			_tzset();
			tzflag++;
		}
		tz->tz_minuteswest = _timezone / 60;
		tz->tz_dsttime = _daylight;
	}

	return 0;
}
#endif






