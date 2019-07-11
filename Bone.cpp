#include "Bone.h"



Bone::Bone()
{
}


Bone::~Bone()
{
}

void Bone::buildBone()
{
	this->program = BuildShader("bone.vert", "bone.frag");

	//Init coordinate
	GLfloat vertices[] = {
		200.0f, -200.0f,
		100.0f, -100.0f
	};

	//init vao
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//init vbo
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//Get attrib
	GLint pos_attrib = glGetAttribLocation((this->program), "linePos");

	glVertexAttribPointer(pos_attrib, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(0 * sizeof(GLfloat)));
	glEnableVertexAttribArray(pos_attrib);

	//unbind
	glBindVertexArray(0);

	GLfloat aspect = (GLfloat)height / (GLfloat)width;

	p = ortho(0.0f, static_cast<GLfloat>(width), static_cast<GLfloat>(height), 0.0f);
	v = translate(mat4(1.0f), vec3(0, 0, 0));
	m = translate(mat4(1.0f), vec3(0, 0, 0));
	glUniformMatrix4fv(glGetUniformLocation(this->program, "view"), 1, GL_FALSE, value_ptr(v));
	glUniformMatrix4fv(glGetUniformLocation(this->program, "projection"), 1, GL_FALSE, value_ptr(p));
	glUniformMatrix4fv(glGetUniformLocation(this->program, "model"), 1, GL_FALSE, value_ptr(m));
}

void Bone::drawBone(ColorSpacePoint startPoint, ColorSpacePoint endPoint)
{
	GLfloat updateVertices[] = {
		startPoint.X, startPoint.Y,
		endPoint.X, endPoint.Y
	};

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(updateVertices), updateVertices, GL_STATIC_DRAW);

	glBindVertexArray(vao);
	glDrawArrays(GL_LINES, 0, 2);
	glEnable(GL_LINE_SMOOTH);
	glLineWidth(8.0f);
}

void Bone::Init()
{
}

void Bone::Render()
{
}

void Bone::Update(float deltaTime)
{
}

void Bone::Input()
{
}
