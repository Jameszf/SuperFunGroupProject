#region vertex
#version 410 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 textureCoord;

uniform mat4 model;
uniform mat4 ortho;

out vec2 tcf;

void main() {
	gl_Position = ortho * model * vec4(pos, 1.0f);
	tcf = textureCoord;
}


#region fragment
#version 410 core

out vec4 color;
in vec3 fragmentColor;
in vec2 tcf;

uniform vec3 objColor;
uniform vec3 rgb;
uniform sampler2D inTexture;
uniform int textureExists;

void main()
{
	
	if (textureExists == 1)
	{
		color = texture(inTexture, tcf);
	}
	else
	{
		color = vec4(rgb, 1.0f);
	}
};