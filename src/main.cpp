#include <stdio.h>
#include <stdlib.h>
#include "Application.hpp"
#include "Settings.hpp"
#include "Limelight.h"
#include "InputController.hpp"
#include "GameStreamClient.hpp"
#include "Logger.hpp"
#include "MouseController.hpp"
#include "KeyboardController.hpp"
#include <glad/glad.h>
#include <switch.h>

#include <GLFW/glfw3.h>

GLFWgamepadstate glfw_gamepad_state;
int moonlight_exit = 0;

static int m_width, m_height, m_fb_width, m_fb_height;

int main(int argc, const char * argv[]) {
    glfwInit();
    
    #ifdef __SWITCH__
    appletSetFocusHandlingMode(AppletFocusHandlingMode_NoSuspend);
    
    bool recording_supported = false;
    appletIsGamePlayRecordingSupported(&recording_supported);
    
    if (recording_supported) {
        appletInitializeGamePlayRecording();
    }
    
    Settings::settings()->set_working_dir("sdmc:/switch/moonlight");
    #else
    Settings::settings()->set_working_dir("/Users/rock88/Documents/Projects/Switch/moonlight-nx/working_dir");
    #endif
    
    Logger::info("Moonlight", "Starting...");
    
    glfwSetErrorCallback([](int i, const char *error) {
        Logger::error("GLFW", "error: %s", error);
    });
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Test", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    
    glfwGetWindowSize(window, &m_width, &m_height);
    glfwGetFramebufferSize(window, &m_fb_width, &m_fb_height);
    
    glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int width, int height) {
        m_width = width;
        m_height = height;
        glfwGetFramebufferSize(window, &m_fb_width, &m_fb_height);
        nanogui::window_resize_callback_event(width, height, m_fb_width, m_fb_height);
    });
    
    GameStreamClient::instance().start();
    
    MouseController::instance().init(window);
    KeyboardController::instance().init(window);
    
    nanogui::init();
    nanogui::ref<Application> app = new Application(Size(m_width, m_height), Size(m_fb_width, m_fb_height));
    
    nanogui::setup(1.0 / 15.0);
    
    while (!glfwWindowShouldClose(window) && !moonlight_exit) {
        glfwPollEvents();
        
        #ifdef __SWITCH__
            glfwGetGamepadState(GLFW_JOYSTICK_1, &glfw_gamepad_state);
            InputController::controller()->handle_gamepad_event(glfw_gamepad_state);
        #endif
        
        MouseController::instance().handle_mouse();
        KeyboardController::instance().handle_keyboard();
        
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        
        nanogui::draw();
        
        //MouseController::instance().draw_cursor(app);
        
        glfwSwapBuffers(window);
    }
    
    GameStreamClient::instance().stop();
    nanogui::leave();
    nanogui::shutdown();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
