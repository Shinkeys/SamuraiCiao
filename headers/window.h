#pragma once

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>



struct Keys
{
	bool front{ false };
	bool back { false };
	bool left { false };
	bool right{ false };
    bool jump { false };
    // if true switch camera look from light position
    bool sceneEditor { false };

    // If some object selected, gizmo is displayed. This variable is used to cancel it
    bool cancelSelection { false };
};

struct Mouse
{
	double displacementX{ 0.0 };
	double displacementY{ 0.0 };

    double xPos { 0.0 };
    double yPos { 0.0 };

    bool mouseMiddle { false };
    bool mouseRight  { false };

    bool mouseLeft   { false };
};

class Window
{
private:
    const char* _title = "Samurai Ciao";
    static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
    static void KeyCallback(GLFWwindow* window, int key, int scan, int action, int mods);
    static void CursorPosCallback(GLFWwindow* window, double xPos, double yPos);
    static void MouseClickCallback(GLFWwindow* window, int button, int action, int mods);
    void ProceedMouseMovement(double xPos, double yPos);
    void ProceedMousePress(int button, int action, int mods);
    
    virtual void Cleanup();
    virtual void Update() = 0;
    virtual void Render() = 0;

    void HandleMouseSpin();
    
    // inputs
    Keys _keys;
	Mouse _mouse;
    float _lastMouseWidth;
	float _lastMouseHeight;
    void ProceedKeys(int key);
    void ResetKey(int key);
    
protected:
    GLFWwindow* _window;
    uint32_t _width { 2560 };
    uint32_t _height{ 1440 };
    virtual bool Initialize();
    Window* GetWindowPointer()       const { return reinterpret_cast<Window*>(glfwGetWindowUserPointer(Window::_window));}
    
public:
    void EnableCursor();
    void DisableCursor();
    uint32_t GetWindowWidth()        const  { return _width; }
    uint32_t GetWindowHeight()       const  { return _height;}
	Keys GetKeysState()              const  { return _keys;  }
    void ResetMouse();
    Mouse GetMouseState()            const  { return _mouse; }
    Window();
    void Run();
    virtual ~Window();
};