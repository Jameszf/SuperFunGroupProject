#ifndef RENDERING_TOOL_IMPORTED
#define RENDERING_TOOL_IMPORTED
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <ft2build.h>
#include FT_FREETYPE_H
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
#include <ctime>

int RTL_WINDOW_WIDTH = 0;
int RTL_WINDOW_HEIGHT = 0;
int RTL_UNIFORM_T_OFFSET_LOC, RTL_UNIFORM_RGB_OFFSET_LOC, RTL_UNIFORM_T_BOOL_LOC, RTL_UNIFORM_T_MATRIX_LOC;

glm::mat4 rtlScaleMatrix = glm::mat4(1.0f);
glm::mat4 rtlRotMatrix = glm::mat4(1.0f);

GLFWwindow* RTL_WINDOW = NULL;

int rtlMouseX, rtlMouseY;

namespace glE
{
	void parseShader(std::string path, std::string* ptrToVertexShaderSrc, std::string* ptrToFragmentShaderSrc)
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

void rtlCursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
	rtlMouseX = (int)xpos;
	rtlMouseY = (int)ypos;
}

namespace rtl
{
	struct sprite
	{
		int size = 0;
		unsigned int ID = 0;
		unsigned int tID = 0;
		float r = 0.f, g = 0.f, b = 0.f;
		sprite(unsigned int bufferID, int sizeInBytes, glm::vec3 rgb, unsigned int textureID = NULL)
		{
			this->size = sizeInBytes;
			this->ID = bufferID;
			this->tID = textureID;
			this->r = rgb[0];
			this->g = rgb[1];
			this->b = rgb[2];
		}
	};

	void terminate()
	{
		glfwTerminate();
	}

	GLFWwindow* init(int width, int height, bool fullscreen, const char * windowName)
	{
		stbi_set_flip_vertically_on_load(true);
		srand((unsigned)time(0));
		RTL_WINDOW_HEIGHT = height;
		RTL_WINDOW_WIDTH = width;
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_SAMPLES, 4);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
		if (fullscreen == true)
			RTL_WINDOW = glfwCreateWindow(width, height, windowName, glfwGetPrimaryMonitor(), NULL);
		else
			RTL_WINDOW = glfwCreateWindow(width, height, windowName, NULL, NULL);
		glfwMakeContextCurrent(RTL_WINDOW);
		if (!RTL_WINDOW)
		{
			std::cout << "Failed to init GLFW window" << std::endl;
			glfwTerminate();
			return NULL;
		}
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD" << std::endl;
			return NULL;
		}
		glViewport(0, 0, width, height);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		unsigned int shaderID = glE::shaderInit("res/shaders/basic.shader");
		glUseProgram(shaderID);
		glfwSetCursorPosCallback(RTL_WINDOW, rtlCursorPosCallback);
		RTL_UNIFORM_T_OFFSET_LOC = glGetUniformLocation(shaderID, "transm");
		RTL_UNIFORM_RGB_OFFSET_LOC = glGetUniformLocation(shaderID, "rgb");
		RTL_UNIFORM_T_BOOL_LOC = glGetUniformLocation(shaderID, "textureExists");
		RTL_UNIFORM_T_MATRIX_LOC = glGetUniformLocation(shaderID, "transformationMatrix");
		return RTL_WINDOW;
	}

	sprite loadTexture(const char* path, bool transparency = false)
	{
		unsigned int textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		int width, height, nr;
		unsigned char* data = stbi_load(path, &width, &height, &nr, 0);
		if (data)
		{
			if (transparency)
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			else
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}
		stbi_image_free(data);
		float vx1, vy1, vx2, vy2, vx3, vy3, vx4, vy4;
		vx1 = 0.0f;
		vy1 = 0.0f;
		vx2 = 0.0f;
		vy2 = -(float)height / (float)RTL_WINDOW_HEIGHT * 2;
		vx3 = (float)width / (float)RTL_WINDOW_WIDTH * 2;
		vy3 = 0.0f;
		vx4 = (float)width / (float)RTL_WINDOW_WIDTH * 2;
		vy4 = -(float)height / (float)RTL_WINDOW_HEIGHT * 2;

		float vert[18] =
		{
			vx1, vy1, 0.0f,
			vx2, vy2, 0.0f,
			vx3, vy3, 0.0f,

			vx3, vy3, 0.0f,
			vx2, vy2, 0.0f,
			vx4, vy4, 0.0f
		};

		float tc[18] =
		{
			0.0f, 1.0f,
			0.0f, 0.0f,
			1.0f, 1.0f,

			1.0f, 1.0f,
			0.0f, 0.0f,
			1.0f, 0.0f,
		};

		unsigned int VAO, VBO, TCBO;
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vert), vert, GL_STATIC_DRAW);

		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glGenBuffers(1, &TCBO);
		glBindBuffer(GL_ARRAY_BUFFER, TCBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(tc), tc, GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
		return sprite(VAO, (int)sizeof(vert), glm::vec3(), textureID);
	}

	void draw(sprite sprite, int x, int y)
	{
		glBindVertexArray(sprite.ID);
		glm::mat4 tm(1.0f);
		tm[3][0] = x / (float)RTL_WINDOW_WIDTH * 2.f - 1.f;
		tm[3][1] = -(y / (float)RTL_WINDOW_HEIGHT * 2.f - 1.f);
		glUniformMatrix4fv(RTL_UNIFORM_T_OFFSET_LOC, 1, GL_FALSE, &tm[0][0]);
		if (sprite.tID != 0)
		{
			glBindTexture(GL_TEXTURE_2D, sprite.tID);
			glUniform1i(RTL_UNIFORM_T_BOOL_LOC, 1);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
		else
		{
			glUniform1i(RTL_UNIFORM_T_BOOL_LOC, 0);
			glUniform3f(RTL_UNIFORM_RGB_OFFSET_LOC, sprite.r, sprite.g, sprite.b);
		}
		glm::mat4 rm = rtlScaleMatrix * rtlRotMatrix;
		glUniformMatrix4fv(RTL_UNIFORM_T_MATRIX_LOC, 1, GL_FALSE, &rm[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	glm::vec2 getMousePos()
	{
		return glm::vec2(rtlMouseX, rtlMouseY);
	}

	bool shouldExit()
	{
		return glfwWindowShouldClose(RTL_WINDOW);
	}

	void clearBuffers()
	{
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void poll()
	{
		glfwPollEvents();
	}

	void flipDisplay()
	{
		glfwSwapBuffers(RTL_WINDOW);
	}

	void setScaleMatrix(float x = 1.0f, float y = 1.0f)
	{
		rtlScaleMatrix = glm::mat4(1.0f);
		rtlScaleMatrix[0][0] = x;
		rtlScaleMatrix[1][1] = y;
	}

	void setRotationMatrix(float deg)
	{

	}

	bool getKey(int glfwKeyCode)
	{
		return glfwGetKey(RTL_WINDOW, glfwKeyCode);
	}

	sprite rect(int width, int height, glm::vec3 rgb)
	{
		float vx1, vy1, vx2, vy2, vx3, vy3, vx4, vy4;
		vx1 = 0.0f;
		vy1 = 0.0f;
		vx2 = 0.0f;
		vy2 = -(float)height / (float)RTL_WINDOW_HEIGHT * 2;
		vx3 = (float)width / (float)RTL_WINDOW_WIDTH * 2;
		vy3 = 0.0f;
		vx4 = (float)width / (float)RTL_WINDOW_WIDTH * 2;
		vy4 = -(float)height / (float)RTL_WINDOW_HEIGHT * 2;

		float vert[18] =
		{
			vx1, vy1, 0.0f,
			vx2, vy2, 0.0f,
			vx3, vy3, 0.0f,

			vx3, vy3, 0.0f,
			vx2, vy2, 0.0f,
			vx4, vy4, 0.0f
		};

		glm::vec3 rgbc = glm::vec3(rgb[0] / 255.f, rgb[1] / 255.f, rgb[2] / 255.f);

		unsigned int VAO, VBO;
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vert), vert, GL_STATIC_DRAW);

		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		return sprite(VAO, (int)sizeof(vert), rgbc);
	}
}

#endif