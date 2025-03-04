#pragma once
#include <iostream>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <glm/glm.hpp>


namespace SamuraiInterface
{
    const float g_windowWidth = 400.0f;
    const float g_windowHeight = 350.0f;
    void InitImgui(GLFWwindow* window);
    void UpdateImgui(const uint32_t viewportWidth, const uint32_t viewportHeight);
    void RenderImgui();
    void DestroyImgui();
    void DebugWindow(glm::vec3 camPos);
};  