#include "../headers/window.h"
#include "../headers/systems/interface.h"

Window::Window()
{
}

Window::~Window()
{
    Cleanup();
}

bool Window::Initialize()
{
    if(!glfwInit())
    {
        std::cerr << "Unable to initialize GLFW\n";
        return false;
    }

    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* videoMode = glfwGetVideoMode(primaryMonitor);
    _width = static_cast<uint32_t>(videoMode->width);
    _height = static_cast<uint32_t>(videoMode->height);

    _lastMouseHeight = _height / 2;
    _lastMouseWidth = _width / 2;


    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_OPENGL_CORE_PROFILE);
#ifndef NDEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif
    _window = glfwCreateWindow(_width, _height, _title, nullptr, nullptr);

    if(_window == nullptr)
    {
        std::cerr << "Unable to create GLFW window\n";
        glfwTerminate();
        return false;
    }


    glfwMakeContextCurrent(_window);
    // initialize GLAD
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Unable to find GLAD\n";
        glfwTerminate();
        return false;
    }

    glfwSetWindowUserPointer(_window, this);
    glfwSetFramebufferSizeCallback(_window, FramebufferSizeCallback);
    glfwSetCursorPosCallback(_window, CursorPosCallback);
    glfwSetKeyCallback(_window, KeyCallback);
    glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    return true;
}

void Window::ResetMouse()
{
	_mouse.x = 0.0f;
	_mouse.y = 0.0f;
}

void Window::ProceedMouseMovement(double xPos, double yPos)
{
    _mouse.x = xPos - _lastMouseWidth;
	_mouse.y = yPos - _lastMouseHeight;

	_lastMouseWidth = xPos;
	_lastMouseHeight = yPos;
}
void Window::CursorPosCallback(GLFWwindow* window, double xPos, double yPos)
{
    Window* app = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
    app->ProceedMouseMovement(xPos, yPos);
}

void Window::FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    Window* app = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
    app->_width = width;
    app->_height = height;
    glViewport(0, 0, width, height);
}
void Window::ProceedKeys(int key)
{
	if (key == GLFW_KEY_W)
	{
		_keys.front = true;
	}
	if (key == GLFW_KEY_S)
	{
		_keys.back = true;
	}
	if (key == GLFW_KEY_A)
	{
		_keys.left = true;
	}
	if (key == GLFW_KEY_D)
	{
		_keys.right = true;
	}
}
void Window::ResetKey(int key)
{
	switch (key)
	{
	case GLFW_KEY_W: _keys.front = false; break;
	case GLFW_KEY_S: _keys.back = false; break;
	case GLFW_KEY_A: _keys.left = false; break;
	case GLFW_KEY_D: _keys.right = false; break;
	}
}

void Window::KeyCallback(GLFWwindow* window, int key, int scan, int action, int mods)
{
    Window* app =  reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
    if(action == GLFW_PRESS && key == GLFW_KEY_ESCAPE)
    {
        glfwSetWindowShouldClose(window, true);
    }

	if (action == GLFW_PRESS)
	{
		app->ProceedKeys(key);
	}
	if (action == GLFW_RELEASE)
	{
		app->ResetKey(key);
	}
}

void Window::Run()
{
    if(!Initialize())
    {
        std::cerr << "Unable to initialize window\n";
        return;
    }
    while(!glfwWindowShouldClose(_window))
    {
        Update();
        SamuraiInterface::UpdateImgui();
        Render();
    }
}

void Window::Cleanup()
{
    if(_window != nullptr)
    {
        glfwDestroyWindow(_window);
        _window = nullptr;
        std::cout << "Window destroyed\n";
    }
    SamuraiInterface::DestroyImgui();
    glfwTerminate();
}