#pragma once
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <gladsource.h>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <iostream>


namespace glE
{
	void parseShader(std::string path, std::string *ptrToVertexShaderSrc, std::string *ptrToFragmentShaderSrc)
	{
		std::string* ptvss = ptrToVertexShaderSrc;
		std::string* ptfss = ptrToFragmentShaderSrc;
		std::string line;
		std::ifstream stream;
		std::stringstream stringStream[2];
		int vOF = 0;
		stream.open(path);
		while (getline(stream, line))
		{
			if (line.find("#region") != std::string::npos)
			{
				if (line.find("vertex") != std::string::npos)
				{
					vOF = 0;
				}
				else if (line.find("fragment") != std::string::npos)
				{
					vOF = 1;
				}
			}
			else
			{
				stringStream[vOF] << line << '\n';
			}
		}
		*ptvss = stringStream[0].str();
		*ptfss = stringStream[1].str();
	}

	unsigned int shaderInit(const char* path) {
		unsigned int vSID = glCreateShader(GL_VERTEX_SHADER);
		unsigned int fSID = glCreateShader(GL_FRAGMENT_SHADER);
		std::string vSSrc;
		std::string fSSrc;
		parseShader(path, &vSSrc, &fSSrc);
		GLint success = GL_FALSE;
		int InfoLogLength;
		char const* VertexSourcePointer = vSSrc.c_str();
		glShaderSource(vSID, 1, &VertexSourcePointer, NULL);
		glCompileShader(vSID);
		glGetShaderiv(vSID, GL_COMPILE_STATUS, &success);
		glGetShaderiv(vSID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0) {
			std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
			glGetShaderInfoLog(vSID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
			std::cout << &VertexShaderErrorMessage[0];
		}
		char const* FragmentSourcePointer = fSSrc.c_str();
		glShaderSource(fSID, 1, &FragmentSourcePointer, NULL);
		glCompileShader(fSID);
		glGetShaderiv(fSID, GL_COMPILE_STATUS, &success);
		glGetShaderiv(fSID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0) {
			std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
			glGetShaderInfoLog(fSID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
			std::cout << &FragmentShaderErrorMessage[0];
		}
		unsigned int ProgramID = glCreateProgram();
		glAttachShader(ProgramID, vSID);
		glAttachShader(ProgramID, fSID);
		glLinkProgram(ProgramID);
		glGetProgramiv(ProgramID, GL_LINK_STATUS, &success);
		glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0) {
			std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
			glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
			std::cout << &ProgramErrorMessage[0];
		}

		glDetachShader(ProgramID, vSID);
		glDetachShader(ProgramID, fSID);

		glDeleteShader(vSID);
		glDeleteShader(fSID);

		return ProgramID;
	}
}