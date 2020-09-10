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
#include <random>
#include "sysec.hpp"

double rtlCurrentTime, rtlDeltaTime, rtlLastTime;

int rtlWindowWidth = 0;
int rtlWindowHeight = 0;
int RTL_UNIFORM_T_ORTHO_LOC, RTL_UNIFORM_RGB_OFFSET_LOC, RTL_UNIFORM_T_BOOL_LOC, RTL_UNIFORM_T_MODELM_LOC;

glm::mat4 rtlScaleMatrix = glm::mat4(1.0f);
glm::mat4 rtlRotMatrix = glm::mat4(1.0f);
glm::mat4 rtlOrthoMatrix;

GLFWwindow* RTL_WINDOW = NULL;
FT_Library RTL_FREETYPE;
FT_Face RTL_FACE;

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
			std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1L);
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
		unsigned int ID = 0;
		unsigned int tID = 0;
		float r = 0.f, g = 0.f, b = 0.f;
		float width = 0, height = 0;
		float angle = 0;
		glm::vec3 scale = glm::vec3(1.f, 1.f, 1.f);
		glm::vec2 pos = glm::vec2(0.f, 0.f);
		glm::vec2 pointOfRot = glm::vec2(0, 0);
		sprite(unsigned int bufferID, glm::vec3 rgb, float width, float height, unsigned int textureID = NULL)
		{
			this->ID = bufferID;
			this->tID = textureID;
			this->r = rgb[0];
			this->g = rgb[1];
			this->b = rgb[2];
			this->width = width;
			this->height = height;
		}
		glm::vec2 getCenter()
		{
			return glm::vec2(scale[0] * width / 2, scale[1] * height / 2);
		}
	};

	void terminate()
	{
		glfwTerminate();
	}

	void init(int width, int height, bool fullscreen, const char * windowName)
	{
		stbi_set_flip_vertically_on_load(true);
		std::srand((unsigned)time(0));
		rtlWindowHeight = height;
		rtlWindowWidth = width;
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
		}
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD" << std::endl;
		}
		glViewport(0, 0, width, height);
		glEnable(GL_BLEND);
		glEnable(GL_MULTISAMPLE);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		unsigned int shaderID = glE::shaderInit("res/shaders/basic.shader");
		glUseProgram(shaderID);
		glfwSetCursorPosCallback(RTL_WINDOW, rtlCursorPosCallback);
		RTL_UNIFORM_T_ORTHO_LOC = glGetUniformLocation(shaderID, "ortho");
		RTL_UNIFORM_RGB_OFFSET_LOC = glGetUniformLocation(shaderID, "rgb");
		RTL_UNIFORM_T_BOOL_LOC = glGetUniformLocation(shaderID, "textureExists");
		RTL_UNIFORM_T_MODELM_LOC = glGetUniformLocation(shaderID, "model");
		rtlOrthoMatrix = glm::ortho(0.0f, (float)rtlWindowWidth, (float)rtlWindowHeight, 0.0f, -1.0f, 1.f);
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

		float vert[18] =
		{
			1.f, 1.f, 0.f,
			1.f, 0.f, 0.f,
			0.f, 0.f, 0.f,

			0.f, 0.f, 0.f,
			1.f, 1.f, 0.f,
			0.f, 1.f, 0.f,
		};

		float tc[12] =
		{
			1.0f, 0.0f,
			1.0f, 1.0f,
			0.0f, 1.0f,

			0.f, 1.f,
			1.f, 0.f,
			0.f, 0.f,
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
		return sprite(VAO, glm::vec3(), width, height, textureID);
	}

	void draw(sprite sprite, int x, int y)
	{
		glBindVertexArray(sprite.ID);
		if (sprite.tID == 0)
		{
			glUniform1i(RTL_UNIFORM_T_BOOL_LOC, 0);
			glUniform3f(RTL_UNIFORM_RGB_OFFSET_LOC, sprite.r, sprite.g, sprite.b);
		}
		else
		{
			glBindTexture(GL_TEXTURE_2D, sprite.tID);
			glUniform1i(RTL_UNIFORM_T_BOOL_LOC, 1);
		}
		glm::mat4 trans = glm::translate(glm::mat4(1.0f), glm::vec3((float)x, (float)y, 0.f));
		if (sprite.angle != 0)
		{
			trans = glm::translate(trans, glm::vec3(sprite.pointOfRot[0], sprite.pointOfRot[1], 0.f));
			trans = glm::rotate(trans, glm::radians(sprite.angle), glm::vec3(0.0f, 0.0f, 1.0f));
			trans = glm::translate(trans, glm::vec3(-sprite.pointOfRot[0], -sprite.pointOfRot[1], 0.f));
		}
		trans = glm::scale(trans, glm::vec3(sprite.width * sprite.scale[0], sprite.height * sprite.scale[1], 0.f));
		glUniformMatrix4fv(RTL_UNIFORM_T_ORTHO_LOC, 1, GL_FALSE, &rtlOrthoMatrix[0][0]);
		glUniformMatrix4fv(RTL_UNIFORM_T_MODELM_LOC, 1, GL_FALSE, &trans[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	void draw(sprite sprite)
	{
		glBindVertexArray(sprite.ID);
		if (sprite.tID == 0)
		{
			glUniform1i(RTL_UNIFORM_T_BOOL_LOC, 0);
			glUniform3f(RTL_UNIFORM_RGB_OFFSET_LOC, sprite.r, sprite.g, sprite.b);
		}
		else
		{
			glBindTexture(GL_TEXTURE_2D, sprite.tID);
			glUniform1i(RTL_UNIFORM_T_BOOL_LOC, 1);
		}
		glm::mat4 trans = glm::translate(glm::mat4(1.0f), glm::vec3((float)sprite.pos[0], (float)sprite.pos[1], 0.f));
		if (sprite.angle != 0)
		{
			trans = glm::translate(trans, glm::vec3(sprite.pointOfRot[0], sprite.pointOfRot[1], 0.f));
			trans = glm::rotate(trans, glm::radians(sprite.angle), glm::vec3(0.0f, 0.0f, 1.0f));
			trans = glm::translate(trans, glm::vec3(-sprite.pointOfRot[0], -sprite.pointOfRot[1], 0.f));
		}
		trans = glm::scale(trans, glm::vec3(sprite.width * sprite.scale[0], sprite.height * sprite.scale[1], 0.f));
		glUniformMatrix4fv(RTL_UNIFORM_T_ORTHO_LOC, 1, GL_FALSE, &rtlOrthoMatrix[0][0]);
		glUniformMatrix4fv(RTL_UNIFORM_T_MODELM_LOC, 1, GL_FALSE, &trans[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	glm::vec2 getMousePos()
	{
		return glm::vec2(rtlMouseX, rtlMouseY);
	}

	double getTime()
	{
		return glfwGetTime();
	}

	double getDeltaTime()
	{
		rtlDeltaTime = rtlCurrentTime - rtlLastTime;
		return rtlDeltaTime;
	}

	bool shouldExit()
	{
		return glfwWindowShouldClose(RTL_WINDOW);
	}

	void clearBuffers()
	{
		glClear(GL_COLOR_BUFFER_BIT);
		rtlCurrentTime = getTime();
	}

	void poll()
	{
		glfwPollEvents();
		rtlLastTime = rtlCurrentTime;
	}

	void setVSync(bool onOrOff)
	{
		bool b = onOrOff;
		if (b)
		{
			glfwSwapInterval(1);
		}
		else
		{
			glfwSwapInterval(0);
		}
	}

	void flipDisplay()
	{
		glfwSwapBuffers(RTL_WINDOW);
	}

	void setClearColor(float r, float g, float b)
	{
		glClearColor(r / 255.f, g / 255.f, b / 255.f, 1.f);
	}

	void setWindowSize(int width, int height)
	{
		rtlWindowWidth = width;
		rtlWindowHeight = height;
		glfwSetWindowSize(RTL_WINDOW, width, height);
		glViewport(0, 0, width, height);
		rtlOrthoMatrix = glm::ortho(0.0f, (float)rtlWindowWidth, (float)rtlWindowHeight, 0.0f, .0f, 1.f);
	}

	bool getKey(int glfwKeyCode)
	{
		return glfwGetKey(RTL_WINDOW, glfwKeyCode);
	}

	int rand(int minInclusive, int maxInclusive)
	{
		if (maxInclusive - minInclusive <= 0)
		{
			throw std::invalid_argument("difference between a and b is negative");
		}
		int k = std::rand() % (maxInclusive - minInclusive + 1) + minInclusive;
		return k;
	}

	bool rdb()
	{
		return std::rand() % 2;
	}

	sprite rect(int width, int height, glm::vec3 rgb)
	{
		float vert[18] =
		{
			1.f, 1.f, 0.f,
			1.f, 0.f, 0.f,
			0.f, 0.f, 0.f,

			0.f, 0.f, 0.f,
			1.f, 1.f, 0.f,
			0.f, 1.f, 0.f,
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
		return sprite(VAO, rgbc, width, height);
	}
}

#endif