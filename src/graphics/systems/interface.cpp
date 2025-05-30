#include "../../headers/systems/interface.h"
#include "../../headers/window.h"

uint32_t SamuraiInterface::g_windowWidth = 0;
uint32_t SamuraiInterface::g_windowHeight = 0;


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

    ChangeDecorations();

}

void SamuraiInterface::UpdateImgui(uint32_t viewportWidth, uint32_t viewportHeight)
{
    ImGuiIO& io = ImGui::GetIO();
    if(io.DisplaySize.x != viewportWidth || io.DisplaySize.y != viewportHeight)
        io.DisplaySize = ImVec2{static_cast<float>(viewportWidth), static_cast<float>(viewportHeight)};

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::Begin("Samurai debug");


}

void SamuraiInterface::DebugWindow(uint32_t windowWidth, uint32_t windowHeight, glm::vec3 camPos)
{
    // starting one window there, to push all data to it
    ImGui::SetNextWindowPos(ImVec2{0.0f,0.0f});
    ImGui::SetNextWindowSize(ImVec2{static_cast<float>(windowWidth), static_cast<float>(windowHeight)});

    g_windowWidth = windowWidth;
    g_windowHeight = windowHeight;

    ImGui::Text("Camera position: %f %f %f", camPos.x, camPos.y, camPos.z);
    // ending window name Samurai Debug

}

void SamuraiInterface::ChangeDecorations()
{
    ImGuiStyle& style = ImGui::GetStyle();

    style.WindowMinSize        = ImVec2( 160.0f, 20.0f );
    style.FramePadding         = ImVec2( 7.0f, 4.0f );
    style.ItemSpacing          = ImVec2( 6.0f, 2.0f );
    style.ItemInnerSpacing     = ImVec2( 6.0f, 4.0f );
    style.Alpha                = 0.95f;
    style.WindowRounding       = 4.0f;
    style.FrameRounding        = 2.0f;
    style.IndentSpacing        = 6.0f;
    style.ItemInnerSpacing     = ImVec2( 2.0f, 4.0f );
    style.ColumnsMinSpacing    = 50.0f;
    style.GrabMinSize          = 14.0f;
    style.GrabRounding         = 16.0f;
    style.ScrollbarSize        = 12.0f;
    style.ScrollbarRounding    = 16.0f;
    

    style.Colors[ImGuiCol_Text]                  = ImVec4(1.0f,  1.0f,  1.0f,  1.0f);
    style.Colors[ImGuiCol_TextDisabled]          = ImVec4(0.86f, 0.93f, 0.89f, 0.28f);
    style.Colors[ImGuiCol_WindowBg]              = ImVec4(0.13f, 0.14f, 0.17f, 1.00f);
    style.Colors[ImGuiCol_Border]                = ImVec4(0.31f, 0.31f, 1.00f, 0.00f);
    style.Colors[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_FrameBg]               = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.18f, 0.92f, 0.25f, 0.78f);
    style.Colors[ImGuiCol_FrameBgActive]         = ImVec4(0.18f, 0.92f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_TitleBg]               = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.20f, 0.22f, 0.27f, 0.75f);
    style.Colors[ImGuiCol_TitleBgActive]         = ImVec4(0.18f, 0.92f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_MenuBarBg]             = ImVec4(0.20f, 0.22f, 0.27f, 0.47f);
    style.Colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.09f, 0.15f, 0.16f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.18f, 0.92f, 0.25f, 0.78f);
    style.Colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.18f, 0.92f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_CheckMark]             = ImVec4(0.05f, 0.75f, 0.15f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab]            = ImVec4(0.47f, 0.77f, 0.83f, 0.14f);
    style.Colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.18f, 0.92f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_Button]                = ImVec4(0.47f, 0.77f, 0.83f, 0.14f);
    style.Colors[ImGuiCol_ButtonHovered]         = ImVec4(0.18f, 0.92f, 0.25f, 0.86f);
    style.Colors[ImGuiCol_ButtonActive]          = ImVec4(0.18f, 0.92f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_Header]                = ImVec4(0.18f, 0.92f, 0.25f, 0.76f);
    style.Colors[ImGuiCol_HeaderHovered]         = ImVec4(0.18f, 0.92f, 0.25f, 0.86f);
    style.Colors[ImGuiCol_HeaderActive]          = ImVec4(0.18f, 0.92f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_Separator]             = ImVec4(0.14f, 0.16f, 0.19f, 1.00f);
    style.Colors[ImGuiCol_SeparatorHovered]      = ImVec4(0.18f, 0.92f, 0.25f, 0.78f);
    style.Colors[ImGuiCol_SeparatorActive]       = ImVec4(0.18f, 0.92f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_ResizeGrip]            = ImVec4(0.47f, 0.77f, 0.83f, 0.04f);
    style.Colors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.18f, 0.92f, 0.25f, 0.78f);
    style.Colors[ImGuiCol_ResizeGripActive]      = ImVec4(0.18f, 0.92f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_PlotLines]             = ImVec4(0.86f, 0.93f, 0.89f, 0.63f);
    style.Colors[ImGuiCol_PlotLinesHovered]      = ImVec4(0.18f, 0.92f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram]         = ImVec4(0.86f, 0.93f, 0.89f, 0.63f);
    style.Colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(0.18f, 0.92f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.18f, 0.92f, 0.25f, 0.43f);
    style.Colors[ImGuiCol_PopupBg]               = ImVec4(0.20f, 0.22f, 0.27f, 0.9f);
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