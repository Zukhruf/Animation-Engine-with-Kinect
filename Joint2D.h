#pragma once
#include "Core.h"
class Joint2D :
	public Core
{
public:
	Joint2D();
	~Joint2D();
	void initJoint();
	void setJointVector(GLfloat x, GLfloat y);
	void drawJoint(ColorSpacePoint & colorPoint);
	void drawJoint(vec4 vectorPerJoint);
	void updateJointPos(vec4 vectorPerJoint);
	vec4 getJointVector();
	mat4 getModelMatrix();

	virtual void Init();
	virtual void Render();
	virtual void Update(float deltaTime);
	virtual void Input();

private:
	GLuint VBO, VAO, EBO, program;
	vec4 vectorJoint;
	mat4 model, view, projection;
};

enum jointDirectory
{
	Head = 0,
	Neck = 1,
	ShoulderLeft = 2,
	ElbowLeft = 3,
	HandLeft = 4,
	ShoulderRight = 5,
	ElbowRight = 6,
	HandRight = 7,
	SpineBase = 8,
	HipLeft = 9,
	KneeLeft = 10,
	AnkleLeft = 11,
	HipRight = 12,
	KneeRight = 13,
	AnkleRight = 14,
	JointDirectory_Count = (AnkleRight + 1)
};