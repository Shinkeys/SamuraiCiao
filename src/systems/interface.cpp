#include "../../headers/systems/interface.h"

void SamuraiInterface::InitImgui(GLFWwindow* window)
{
    // setupping imgui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // setup render backend
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();
}

void SamuraiInterface::UpdateImgui()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void SamuraiInterface::DebugWindow(glm::vec3 camPos)
{
    ImGui::SetNextWindowPos(ImVec2{0.0f,0.0f});
    const float windowWidth = 400.0f;
    const float windowHeight = 150.0f;
    ImGui::SetNextWindowSize(ImVec2{windowWidth, windowHeight});
    ImGui::Begin("Samurai debug");

    ImGui::Text("Camera position: %f %f %f", camPos.x, camPos.y, camPos.z);

    bool temp = false;
    ImGui::Checkbox("Shadows", &temp);

    ImGui::End();
}

void SamuraiInterface::RenderImgui()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void SamuraiInterface::DestroyImgui()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}