#pragma once
#include "Core.h"
class Component2D :
	public Core
{
public:
	Component2D();
	~Component2D();
	void drawComponen();
	void initComponen(string ComponenName);
	void setSize(vec4 size);
	void setPos(GLfloat x, GLfloat y);
	void setPos(ColorSpacePoint & csp);
	void setPosCustomOrigin(GLfloat x, GLfloat y);
	void rotateComponen(GLfloat degree);
	int getWidthScreen();
	int getHeightScreen();
	virtual void Init();
	virtual void Render();
	virtual void Update(float deltaTime);
	virtual void Input();
	virtual void DeInit();

private:
	GLuint VBO, VAO, EBO, texture, program;
	mat4 model, view, projection;
};