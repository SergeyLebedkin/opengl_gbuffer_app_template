#include <iostream>
#include <GLFW/glfw3.h>
#include "InstaApp.hpp"
#include "InstaUtils.hpp"
// ImGui
#include <imgui.h>
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl4.h"

// main
int main(int argc, char** argv) {
	// init window and context
	// set error handling
	glfwSetErrorCallback([](int error, const char* description) {
		std::cout << "Error: " << description << std::endl;
	});
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	GLFWwindow* window = glfwCreateWindow(800, 600, "InstaLOD App", nullptr, nullptr);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);

	// init opengl
	InitOpenGL4();
	// Setup Dear ImGui binding
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::GetIO().IniFilename = nullptr;
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL4_Init();

	// create renderer
	InstaApp* instaApp = new InstaApp();
	instaApp->Init();

	// init timestamp
	TimeStamp timeStamp{};
	timeStamp.Reset();
	
	// main loop
	double mouse_xpos_prev{}, mouse_ypos_prev{};
	glfwGetCursorPos(window, &mouse_xpos_prev, &mouse_ypos_prev);
	while (!glfwWindowShouldClose(window)) {
		// ImGui new frame
		ImGui_ImplGlfw_NewFrame();
		ImGuiIO& io = ImGui::GetIO();

		// update timestamp
		timeStamp.Tick();
		timeStamp.Print(std::cout, 1.0f);

		// camera update
		if (glfwGetKey(window, GLFW_KEY_W) || glfwGetKey(window, GLFW_KEY_UP)) 
			instaApp->mCameraEye += (instaApp->mCameraDir * instaApp->mCameraSpeed * timeStamp.mDeltaTime);
		if (glfwGetKey(window, GLFW_KEY_S) || glfwGetKey(window, GLFW_KEY_DOWN))
			instaApp->mCameraEye -= (instaApp->mCameraDir * instaApp->mCameraSpeed * timeStamp.mDeltaTime);
		if (glfwGetKey(window, GLFW_KEY_A) || glfwGetKey(window, GLFW_KEY_LEFT))
			instaApp->mCameraEye -= (glm::normalize(glm::cross(instaApp->mCameraDir, instaApp->mCameraUp)) * instaApp->mCameraSpeed * timeStamp.mDeltaTime);
		if (glfwGetKey(window, GLFW_KEY_D) || glfwGetKey(window, GLFW_KEY_RIGHT))
			instaApp->mCameraEye += (glm::normalize(glm::cross(instaApp->mCameraDir, instaApp->mCameraUp)) * instaApp->mCameraSpeed * timeStamp.mDeltaTime);
		if (glfwGetKey(window, GLFW_KEY_E))
			instaApp->mCameraEye += (instaApp->mCameraUp * instaApp->mCameraSpeed * timeStamp.mDeltaTime);
		if (glfwGetKey(window, GLFW_KEY_Q))
			instaApp->mCameraEye -= (instaApp->mCameraUp * instaApp->mCameraSpeed * timeStamp.mDeltaTime);

		// mouse update
		double mouse_xpos{}, mouse_ypos{};
		glfwGetCursorPos(window, &mouse_xpos, &mouse_ypos);
		int mouse_state_middle = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE);
		if (mouse_state_middle == GLFW_PRESS) {
			float mouse_xdelta = (float)(mouse_xpos - mouse_xpos_prev);
			float mouse_ydelta = (float)(mouse_ypos - mouse_ypos_prev);
			instaApp->mCameraYaw += mouse_xdelta / 3;
			instaApp->mCameraPitch -= mouse_ydelta / 3;
			if (instaApp->mCameraPitch >= +90) instaApp->mCameraPitch = +89;
			if (instaApp->mCameraPitch <= -90) instaApp->mCameraPitch = -89;
		}
		int mouse_state_left = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
		if (mouse_state_left == GLFW_PRESS)
			instaApp->ApplyBrush();
		mouse_xpos_prev = mouse_xpos;
		mouse_ypos_prev = mouse_ypos;

		// get window properties
		int width{}, height{};
		glfwGetFramebufferSize(window, &width, &height);

		static float brushRadius = 16;
		static glm::vec4 brushColor(1.0f);
		
		// update renderer
		instaApp->SetBrushRadius(brushRadius);
		instaApp->SetBrushColor(brushColor);
		instaApp->SetCursorPosition((int32_t)mouse_xpos, (int32_t)mouse_ypos);
		instaApp->SetViewportSize(width, height);
		instaApp->Update(timeStamp.mDeltaTime);
		instaApp->Render();

		// draw ImGui. it should handled by renderer
		// Start the Dear ImGui frame
		ImGui::NewFrame();
		//ImGui::SetNextWindowPos(ImVec2(0, 0));
		//ImGui::SetNextWindowSize(ImVec2(300, 200));
		ImGui::Begin("Renderer info");
		//ImGui::SliderFloat("float", &brushRadius, 0.0f, 32.0f);
		//ImGui::ColorEdit4("Color", &brushColor.x);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
		ImGui::EndFrame();
		ImGui::Render();

		ImGui_ImplOpenGL4_RenderDrawData(ImGui::GetDrawData());

		// swap buffers and update events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// destroy renderer
	instaApp->Destroy();
	delete instaApp;

	// ImGui clean-up
	ImGui_ImplOpenGL4_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	// destroy window
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}