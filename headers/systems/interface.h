#pragma once
#include <iostream>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <glm/glm.hpp>


namespace SamuraiInterface
{
    extern uint32_t g_windowWidth;
    extern uint32_t g_windowHeight;
    void InitImgui(GLFWwindow* window);
    void UpdateImgui(uint32_t viewportWidth, uint32_t viewportHeight);
    void RenderImgui();
    void DestroyImgui();
    void DebugWindow(uint32_t windowWidth, uint32_t windowHeight, glm::vec3 camPos);
    void ChangeDecorations();
};  