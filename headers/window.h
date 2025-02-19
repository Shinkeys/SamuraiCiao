#pragma once

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>



struct Keys
{
	bool front{ false };
	bool back{ false };
	bool left{ false };
	bool right{ false };
};

struct Mouse
{
	double x{ 0.0 };
	double y{ 0.0 };
};

class Window
{
private:
    const char* _title = "Samurai Ciao";
    static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
    static void KeyCallback(GLFWwindow* window, int key, int scan, int action, int mods);
    static void CursorPosCallback(GLFWwindow* window, double xPos, double yPos);
    void ProceedMouseMovement(double xPos, double yPos);
    virtual void Cleanup();
    virtual void Update() = 0;
    virtual void Render() = 0;

    
    // inputs
    Keys _keys;
	Mouse _mouse;
    float _lastMouseWidth;
	float _lastMouseHeight;
    void ProceedKeys(int key);
    void ResetKey(int key);
    
protected:
    GLFWwindow* _window;
    uint32_t _width{2560};
    uint32_t _height{1440};
    virtual bool Initialize();
    Window* GetWindowPointer() const { return reinterpret_cast<Window*>(glfwGetWindowUserPointer(Window::_window));}
    
public:
    const Mouse& GetLastMouseOffset() const { return _mouse; }
	const Keys GetKeysState() const { return _keys; }
    void ResetMouse();
    Window();
    void Run();
    virtual ~Window();
};