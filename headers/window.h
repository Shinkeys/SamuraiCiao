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
    // if true switch camera look from light position
    bool showImgui{false};
};

struct Mouse
{
	double x{ 0.0 };
	double y{ 0.0 };

    bool clicked{false};
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
    void ProceedMousePress(int action);
    
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
    void EnableCursor();
    void DisableCursor();
    uint32_t GetWindowWidth() const { return _width;}
    uint32_t GetWindowHeight() const { return _height;}
    const Mouse& GetMousePositions() const { return _mouse; }
	Keys GetKeysState() const { return _keys; }
    void ResetMouse();
    Window();
    void Run();
    virtual ~Window();
};