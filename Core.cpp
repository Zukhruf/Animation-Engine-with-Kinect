#include "Core.h"

Core::Core()
{
}

Core::~Core()
{
}

//Start Core
void Core::startCore(string title, bool vsync, WindowFlag windowFlag)
{
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4);

	/*glewExperimental = GL_TRUE;*/
	//Init SDL
	SDL_Init(SDL_INIT_EVERYTHING);

	//Tell SDL Doublebuffer
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	Uint32 flags = SDL_WINDOW_OPENGL;

	if (WindowFlag::EXCLUSIVE_FULLSCREEN == windowFlag) {
		flags |= SDL_WINDOW_FULLSCREEN;
	}

	if (WindowFlag::FULLSCREEN == windowFlag) {
		flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
	}

	if (WindowFlag::BORDERLESS == windowFlag) {
		flags |= SDL_WINDOW_BORDERLESS;
	}

	if (WindowFlag::WINDOWED == windowFlag)
	{
		flags |= SDL_WINDOW_RESIZABLE;
	}

	//Setup SDL Window
	window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);
	if (window == nullptr)
	{
		Err("Failed to create SDL window!");
	}

	glContext = SDL_GL_CreateContext(window);
	if (glContext == nullptr)
	{
		Err("Failed to create SDL_GL context!");
	}

	//Set up gl3w
	GLenum gl3wStat = gl3wInit();
	if (gl3wStat != 0)
	{
		Err("Failed to initialize gl3w!");
	}

	SDL_GL_SetSwapInterval(vsync);

	this->state = State::RUNNING;

	last = SDL_GetTicks();
	Init();

	SDL_version compiled;
	SDL_VERSION(&compiled)

	std::wcout << "Skeletal Animation Engine V4"<< (wchar_t)0xA9 <<" Alpha Version" << std::endl;
	printf("Build with: ");
	std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
	printf("SDL Version: %d.%d.%d \n\n", compiled.major, compiled.minor, compiled.patch);

	//Own Engine Loop
	while (State::RUNNING == state)
	{
		float deltaTime = GetDeltaTime();
		GetFPS();
		/*PollInput();*/
		Input();
		Update(deltaTime);
		Render();
		SDL_GL_SwapWindow(window);
		LimitFPS();
		PrintFPS();
	}
	DeInit();
}

//Destroy Core Object
void Core::DeInit()
{
	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_GL_DeleteContext(glContext);
	SDL_DestroyWindow(window);
}

//Build Shader
GLuint Core::BuildShader(const char * vertexPath, const char * fragmentPath, const char * geometryPath)
{
	// 1. Retrieve the vertex/fragment source code from filePath
	string vertexCode;
	string fragmentCode;
	string geometryCode;
	ifstream vShaderFile;
	ifstream fShaderFile;
	ifstream gShaderFile;
	// ensures ifstream objects can throw exceptions:
	vShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
	fShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
	gShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
	try
	{
		// Open files
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		stringstream vShaderStream, fShaderStream;
		// Read file's buffer contents into streams
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		// close file handlers
		vShaderFile.close();
		fShaderFile.close();
		// Convert stream into string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
		// If geometry shader path is present, also load a geometry shader
		if (geometryPath != nullptr)
		{
			gShaderFile.open(geometryPath);
			stringstream gShaderStream;
			gShaderStream << gShaderFile.rdbuf();
			gShaderFile.close();
			geometryCode = gShaderStream.str();
		}
	}
	catch (ifstream::failure e)
	{
		Err("ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ");
	}
	const GLchar* vShaderCode = vertexCode.c_str();
	const GLchar * fShaderCode = fragmentCode.c_str();
	// 2. Compile shaders
	GLuint vertex, fragment;
	GLint success;
	GLchar infoLog[512];
	// Vertex Shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	CheckShaderErrors(vertex, "VERTEX");
	// Fragment Shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	CheckShaderErrors(fragment, "FRAGMENT");
	// If geometry shader is given, compile geometry shader
	GLuint geometry;
	if (geometryPath != nullptr)
	{
		const GLchar * gShaderCode = geometryCode.c_str();
		geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry, 1, &gShaderCode, NULL);
		glCompileShader(geometry);
		CheckShaderErrors(geometry, "GEOMETRY");
	}
	// Shader Program
	GLuint program = glCreateProgram();
	glAttachShader(program, vertex);
	glAttachShader(program, fragment);
	if (geometryPath != nullptr)
		glAttachShader(program, geometry);
	glLinkProgram(program);
	CheckShaderErrors(program, "PROGRAM");
	// Delete the shaders as they're linked into our program now and no longer necessery
	glDeleteShader(vertex);
	glDeleteShader(fragment);
	if (geometryPath != nullptr)
		glDeleteShader(geometry);
	return program;
}

//Use Shader
void Core::UseShader(GLuint program)
{
	// Uses the current shader
	glUseProgram(program);
}

//Get Delta Time
float Core::GetDeltaTime()
{
	unsigned int time = SDL_GetTicks();
	unsigned int delta = time - lastFrame;
	lastFrame = time;

	return delta;
}

//Get FPS
void Core::GetFPS()
{
	if (SDL_GetTicks() - last > 1000) {
		fps = _fps;
		_fps = 0;
		last += 1000;
	}
	_fps++;
}

//Input Poll
void Core::PollInput()
{
	SDL_Event evt;
	//Will keep looping until there are no more events to process
	while (SDL_PollEvent(&evt)) {
		switch (evt.type) {
		case SDL_QUIT:
			state = State::EXIT;
			break;
		case SDL_KEYDOWN:
			switch (evt.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				state = State::EXIT;
				break;
			case SDLK_SPACE:
				std::cout << "Space ditekan!" << std::endl;
			}
		}
	}
}

//Error Check
void Core::Err(string errorString)
{
	cout << errorString << endl;
	SDL_Quit();
	exit(1);
}

//FPS Limitter
void Core::LimitFPS()
{
	//Limit the FPS to the max FPS
	SDL_Delay((Uint32)(targetFrameTime));
}

//Check if Any Shader Error
void Core::CheckShaderErrors(GLuint shader, string type)
{
	GLint success;
	GLchar infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			Err("| ERROR::::SHADER-COMPILATION-ERROR of type: " + type + "|\n" + infoLog + "\n| -- --------------------------------------------------- -- |");
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			Err("| ERROR::::PROGRAM-LINKING-ERROR of type: " + type + "|\n" + infoLog + "\n| -- --------------------------------------------------- -- |");
		}
	}
}

//Print FPS
void Core::PrintFPS()
{
	//print only once every 60 frames
	static int frameCounter = 0;
	frameCounter++;
	if (frameCounter == 60) {
		cout << "FPS: " << fps << endl;
		frameCounter = 0;
	}
}