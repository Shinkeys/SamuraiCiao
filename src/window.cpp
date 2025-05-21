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

    _lastMouseHeight = static_cast<float>(_height) / 2.0f;
    _lastMouseWidth = static_cast<float>(_width) / 2.0f;


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

    //MSAA
    glfwWindowHint(GLFW_SAMPLES, 8);

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
    glfwSetMouseButtonCallback(_window, MouseClickCallback);
    glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    return true;
}

void Window::ResetMouse()
{
	_mouse.displacementX = 0.0f;
	_mouse.displacementY = 0.0f;
}

void Window::MouseClickCallback(GLFWwindow* window, int button, int action, int mods)
{
    Window* app = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
    app->ProceedMousePress(button, action, mods);
    
}

// Purpose: handle mouse spin 360 degrees in editor mode
void Window::HandleMouseSpin()
{
    const auto currInputMode = glfwGetInputMode(_window, GLFW_CURSOR);
    if(_keys.sceneEditor && _mouse.mouseMiddle)
    {
        glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    else if(currInputMode != GLFW_CURSOR_NORMAL && _keys.sceneEditor && !_mouse.mouseMiddle)
    {
        glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}
void Window::ProceedMousePress(int button, int action, int mods)
{
    switch(action)
    {
    case GLFW_PRESS:
        {
            switch(button)
            {
                case GLFW_MOUSE_BUTTON_MIDDLE:
                    _mouse.mouseMiddle = true;
                    break;

                case GLFW_MOUSE_BUTTON_2:
                    _mouse.mouseRight = true;
                    break;
                
                case GLFW_MOUSE_BUTTON_1:
                    _mouse.mouseLeft =  true;
                    break;
                    
            }
        }
        break;

    case GLFW_RELEASE:
        switch(button)
            {
                case GLFW_MOUSE_BUTTON_MIDDLE:
                    _mouse.mouseMiddle = false;
                    break;

                case GLFW_MOUSE_BUTTON_2:
                    _mouse.mouseRight = false;
                    break;

                case GLFW_MOUSE_BUTTON_1:
                    _mouse.mouseLeft = false;
                    break;
            }
        break;
        
    default:
        std::cout << "Unknown action on mouse click\n";
        break;
    }
}

void Window::ProceedMouseMovement(double xPos, double yPos)
{
    _mouse.displacementX = xPos - _lastMouseWidth;
	_mouse.displacementY = yPos - _lastMouseHeight;

    _mouse.xPos = xPos;
    _mouse.yPos = yPos;

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
    if(key == GLFW_KEY_SPACE)
    {
        _keys.jump = true;
    }
    if(key == GLFW_KEY_ESCAPE)
    {
        _keys.cancelSelection = true;
    }

    if(key == GLFW_KEY_F2)
    {
        _keys.sceneEditor = !_keys.sceneEditor;
        if(_keys.sceneEditor)
            EnableCursor();
        else
            DisableCursor();
    }
}
void Window::ResetKey(int key)
{
	switch (key)
	{
	case GLFW_KEY_W: _keys.front = false;               break;
	case GLFW_KEY_S: _keys.back = false;                break;
	case GLFW_KEY_A: _keys.left = false;                break;
	case GLFW_KEY_D: _keys.right = false;               break;
    case GLFW_KEY_SPACE: _keys.jump = false;            break;
    case GLFW_KEY_ESCAPE: _keys.cancelSelection = false;    break;
    default: break;
	}
}

void Window::KeyCallback(GLFWwindow* window, int key, int scan, int action, int mods)
{
    Window* app =  reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
    // if(action == GLFW_PRESS && key == GLFW_KEY_ESCAPE)
    // {
    //     glfwSetWindowShouldClose(window, true);
    // }

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
        HandleMouseSpin();
        Update();
        if(_keys.sceneEditor)
            SamuraiInterface::UpdateImgui(_width, _height);
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


void Window::DisableCursor()
{
    glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Window::EnableCursor()
{
    glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}