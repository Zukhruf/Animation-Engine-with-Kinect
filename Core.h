#pragma once
#include <Windows.h>
#include <Ole2.h>
#include <Kinect.h>
#include <gl3w/GL/gl3w.h>
#include <SOIL/SOIL.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iterator>
#include <nlohmann/json.hpp>
#include <vector>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include <glm/glm/gtx/vector_angle.hpp>
#include <SDL/SDL.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_sdl.h>
//#include <glfw/include/GLFW/glfw3.h>
//#include <glew/include/GL/glew.h>

#pragma region CoreClass
using namespace std;
using namespace glm;
using namespace nlohmann;
using json = nlohmann::json;

enum class State { RUNNING, EXIT };
enum class WindowFlag { WINDOWED, FULLSCREEN, EXCLUSIVE_FULLSCREEN, BORDERLESS };

const int width = 1920;
const int height = 1080;

class Core
{
public:
	Core();
	~Core();
	void startCore(string title, bool vsync, WindowFlag windowFlag);	//

protected:
	virtual void Init() = 0;
	virtual void Render() = 0;
	virtual void Update(float deltaTime) = 0;
	virtual void Input() = 0;
	virtual void DeInit();
	GLuint BuildShader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);
	void UseShader(GLuint program);
	State state;
	float GetDeltaTime();
	SDL_Window* window;
	SDL_GLContext glContext;

private:
	unsigned int screenWidth, screenHeight, lastFrame = 0, last = 0, _fps = 0, fps = 0;
	float targetFrameTime = 0, timeScale;
	void GetFPS();
	void PollInput();
	void Err(string errorString);
	void LimitFPS();
	void CheckShaderErrors(GLuint shader, string type);
	void PrintFPS();
};
#pragma endregion