#include "Component2D.h"

Component2D::Component2D()
{
}

Component2D::~Component2D()
{
}
//Render 2D Sprite
void Component2D::drawComponen()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Bind Textures using texture units
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	// Activate shader
	UseShader(this->program);
	glUniform1i(glGetUniformLocation(this->program, "ourTexture"), 0);

	// Draw sprite
	glBindVertexArray(VAO);

	glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.
	glDisable(GL_BLEND);
}
//Initialize 2D Sprite
void Component2D::initComponen(string ComponenName)
{
	this->program = BuildShader("sprite.vert", "sprite.frag");

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	int heightComponen, widthComponen;

	unsigned char* image = SOIL_load_image(ComponenName.c_str(), &widthComponen, &heightComponen, 0, SOIL_LOAD_RGBA);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widthComponen, heightComponen, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	GLfloat vertices[] = {
		// Positions   // Colors           // Texture Coords
		widthComponen,  heightComponen, 0.0f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f, // Top Right
		widthComponen,  -heightComponen, 0.0f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f, // Bottom Right
		-widthComponen, -heightComponen, 0.0f,   1.0f, 1.0f, 1.0f,   0.0f, 0.0f, // Bottom Left
		-widthComponen, heightComponen, 0.0f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f  // Top Left 
	};

	GLuint indices[] = {  // Note that we start from 0!
		0, 3, 2, 1
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(0 * sizeof(GLfloat)));
	glEnableVertexAttribArray(0);
	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// TexCoord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0); // Unbind VAO

	//Projection
	projection = ortho(0.0f, static_cast<GLfloat>(getWidthScreen()), static_cast<GLfloat>(getHeightScreen()), 0.0f,-1.0f, 1.0f);
	UseShader(this->program);
	glUniformMatrix4fv(glGetUniformLocation(this->program, "projection"), 1, GL_FALSE, value_ptr(projection));

	//View //Camera
	view = translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(this->program, "view"), 1, GL_FALSE, value_ptr(view));
}
//Set Size 
void Component2D::setSize(vec4 size)
{
	UseShader(this->program);
	model = scale(model, vec3(size.x, size.y, size.z));
	glUniformMatrix4fv(glGetUniformLocation(this->program, "model"), 1, GL_FALSE, value_ptr(model));
}
//Set Position
void Component2D::setPos(GLfloat x, GLfloat y)
{
	UseShader(this->program);
	mat4 object = mat4(1.0f);
	model = translate(object, vec3(x, y, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(this->program, "model"), 1, GL_FALSE, value_ptr(model));
}
//Set Position
void Component2D::setPos(ColorSpacePoint & csp)
{
	UseShader(this->program);
	mat4 object = mat4(1.0f);
	model = translate(object, vec3(csp.X, csp.Y, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(this->program, "model"), 1, GL_FALSE, value_ptr(model));
}
//Set Custom Origin
void Component2D::setPosCustomOrigin(GLfloat x, GLfloat y)
{
	UseShader(this->program);
	model = translate(model, vec3(x, y, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(this->program, "model"), 1, GL_FALSE, value_ptr(model));
}
//Rotate Componen
void Component2D::rotateComponen(GLfloat degree)
{
	UseShader(this->program);
	model = rotate(model, degree, vec3(0.0f, 0.0f, 1.0f));
	glUniformMatrix4fv(glGetUniformLocation(this->program, "model"), 1, GL_FALSE, value_ptr(model));
}
//Get Width of Screen
int Component2D::getWidthScreen()
{
	return width;
}
//Get Height of Screen
int Component2D::getHeightScreen()
{
	return height;
}
// ===================================================
// Derived Method
void Component2D::Init()
{
}

void Component2D::Render()
{
}

void Component2D::Update(float deltaTime)
{
}

void Component2D::Input()
{
}

void Component2D::DeInit()
{
}