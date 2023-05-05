// Pubic interface
IMGUI_IMPL_API bool ImGui_ImplOpenGL4_Init();
IMGUI_IMPL_API void ImGui_ImplOpenGL4_Shutdown();
IMGUI_IMPL_API void ImGui_ImplOpenGL4_RenderDrawData(ImDrawData* draw_data);

// Called by Init/NewFrame/Shutdown
IMGUI_IMPL_API bool ImGui_ImplOpenGL4_CreateFontsTexture();
IMGUI_IMPL_API void ImGui_ImplOpenGL4_DestroyFontsTexture();
IMGUI_IMPL_API bool ImGui_ImplOpenGL4_CreateDeviceObjects();
IMGUI_IMPL_API void ImGui_ImplOpenGL4_DestroyDeviceObjects();
