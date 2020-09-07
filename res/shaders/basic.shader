#region vertex
#version 410 core

layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 textureCoord;

uniform mat4 transm;
uniform mat4 transformationMatrix;

out vec2 tcf;

void main() {
	gl_Position = vec4(pos[0] + transm[3][0], pos[1] + transm[3][1], 1.0f, 1.0f) * transformationMatrix;
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