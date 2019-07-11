#include "Joint2D.h"

Joint2D::Joint2D()
{
}

Joint2D::~Joint2D()
{
}
//Initialize Joint
void Joint2D::initJoint()
{
	this->program = BuildShader("point.vert", "point.frag");
	GLfloat vertices_position[] = {
		0.0f, 0.0f
	};

	//VAO Initialize
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//VBO Initialize
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_position), vertices_position, GL_STATIC_DRAW);

	//Get the location of the attribute that enter vertex shader
	GLint pos_attrib = glGetAttribLocation(this->program, "position");

	glVertexAttribPointer(pos_attrib, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(0 * sizeof(GLfloat)));
	glEnableVertexAttribArray(pos_attrib);

	glBindVertexArray(0); // Unbind VAO

	glEnable(GL_DEPTH);

	GLfloat aspect = (GLfloat)height / (GLfloat)width;

	//Projection
	projection = ortho(0.0f, static_cast<GLfloat>(width), static_cast<GLfloat>(height), 0.0f, -1.0f, 1.0f);
	UseShader(this->program);
	//View/Camera
	view = translate(mat4(1.0f), vec3(0, 0, 0));
	glUniformMatrix4fv(glGetUniformLocation(this->program, "view"), 1, GL_FALSE, value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(this->program, "projection"), 1, GL_FALSE, value_ptr(projection));
}
//Set Joint Vector
void Joint2D::setJointVector(GLfloat x, GLfloat y)
{
	vectorJoint.x = x;
	vectorJoint.y = y;
	vectorJoint.z = 0;
	vectorJoint.w = 1;
}
//Render Joint
void Joint2D::drawJoint(ColorSpacePoint & colorPoint)
{
	UseShader(this->program);
	model = translate(mat4(1.0f), vec3(colorPoint.X, colorPoint.Y, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(this->program, "model"), 1, GL_FALSE, value_ptr(model));

	setJointVector(colorPoint.X, colorPoint.Y);

	glBindVertexArray(VAO);
	glDrawArrays(GL_POINTS, 0, 1);
	glEnable(GL_PROGRAM_POINT_SIZE);
}
//Render Joint
void Joint2D::drawJoint(vec4 vectorPerJoint)
{
	UseShader(this->program);
	model = translate(mat4(1.0f), vec3(vectorPerJoint.x, vectorPerJoint.y, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(this->program, "model"), 1, GL_FALSE, value_ptr(model));

	setJointVector(vectorPerJoint.x, vectorPerJoint.y);

	glBindVertexArray(VAO);
	glDrawArrays(GL_POINTS, 0, 1);
	glEnable(GL_PROGRAM_POINT_SIZE);
}
//Update Joint Position
void Joint2D::updateJointPos(vec4 vectorPerJoint)
{
	UseShader(this->program);
	model = translate(mat4(1.0f), vec3(vectorPerJoint.x, vectorPerJoint.y, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(this->program, "model"), 1, GL_FALSE, value_ptr(model));
}
//Get Joint Vector
vec4 Joint2D::getJointVector()
{
	return vectorJoint;
}
//Get Model Matrix
mat4 Joint2D::getModelMatrix()
{
	return model;
}
// =====================================
// Derived Method
void Joint2D::Init()
{
}

void Joint2D::Render()
{
}

void Joint2D::Update(float deltaTime)
{
}

void Joint2D::Input()
{
}