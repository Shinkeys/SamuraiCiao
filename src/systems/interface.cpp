#include "../../headers/systems/interface.h"
#include "../../headers/window.h"

void SamuraiInterface::InitImgui(GLFWwindow* window)
{
    // setupping imgui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.DisplaySize = ImGui::GetMainViewport()->Size;
    // setup render backend
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();
}

void SamuraiInterface::UpdateImgui(const uint32_t viewportWidth, const uint32_t viewportHeight)
{
    ImGuiIO& io = ImGui::GetIO();
    if(io.DisplaySize.x != viewportWidth || io.DisplaySize.y != viewportHeight)
        io.DisplaySize = ImVec2{static_cast<float>(viewportWidth), static_cast<float>(viewportHeight)};

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::Begin("Samurai debug");

}

void SamuraiInterface::DebugWindow(glm::vec3 camPos)
{
    // starting one window there, to push all data to it
    ImGui::SetNextWindowPos(ImVec2{0.0f,0.0f});
    ImGui::SetNextWindowSize(ImVec2{g_windowWidth, g_windowHeight});
    

    ImGui::Text("Camera position: %f %f %f", camPos.x, camPos.y, camPos.z);
    // ending window name Samurai Debug

}


void SamuraiInterface::RenderImgui()
{
    ImGui::End();
    ImGui::SetNextFrameWantCaptureMouse(true);
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void SamuraiInterface::DestroyImgui()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}