#pragma once
#include "Core.h"
class Bone :
	public Core
{
public:
	Bone();
	~Bone();

	void buildBone();
	void drawBone(ColorSpacePoint startPoint, ColorSpacePoint endPoint);

	virtual void Init();
	virtual void Render();
	virtual void Update(float deltaTime);
	virtual void Input();

private:
	mat4 m, v, p;
	GLuint vao, vbo, program;
};

