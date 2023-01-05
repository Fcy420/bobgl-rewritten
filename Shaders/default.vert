#version 330 core

//Positions/Coordinates
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;
layout (location = 2) in float normals;
out vec2 texCoord;
out vec4 color;
uniform mat4 cameraMatrix;
uniform mat4 transformMatrix;
uniform vec3 pos;
uniform vec2 waterPos;
uniform vec4 aColor;
uniform vec3 sunPos;

void main()
{
	vec3 normal;
	if(normals == 0)
		normal = vec3(0,0,-1);
	if(normals == 1)
		normal = vec3(0,0,1);
	if(normals == 2)
		normal = vec3(1,0,0);
	if(normals == 3)
		normal = vec3(-1,0,0);
	if(normals == 4)
		normal = vec3(0,1,0);
	if(normals == 5)
		normal = vec3(0,-1,0);
	float x = aPos.x;
	float y = aPos.y;
	float z = aPos.z;
	float waterHeightX = sin((x+pos.x+waterPos.x));
	float waterHeightY = sin((z+pos.z+waterPos.y));
	float yValue = y;
	if(aColor.a < 1.0f) {
		yValue += (waterHeightX+waterHeightY)*0.2f;
	}
	gl_Position = cameraMatrix * transformMatrix * vec4(x,yValue,z, 1.0);
	texCoord = vec2(aTex.x/10.0f, aTex.y/10.0f);
	color = vec4(aColor.rgb, aColor.a);
	if(normal == vec3(0,0,-1) || normal == vec3(1,0,0) || normal == vec3(0,-1,0)) {
		color = vec4(color.rgb*vec3(0.7,0.7,0.7), color.a);
	}
}