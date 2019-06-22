#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <vector>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>

#include "Car.hpp"

std::vector<int> keys;


void key_callback( GLFWwindow* window, int key, int scancode, int action, int mods ){
    switch (action) {
        case GLFW_PRESS:
            keys.push_back(key);
            break;
            
        case GLFW_RELEASE:
            // if (std::find(keys.begin(), keys.end(), key) != keys.end()){
            keys.erase(std::remove(keys.begin(), keys.end(), key), keys.end());
            // }
            break;
    }
    
    switch (key)
    {
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, GL_TRUE);
            break;
    }
}

void reactOnPressedKeys()
{
    if (keys.size() == 0 || !((std::find(keys.begin(), keys.end(), GLFW_KEY_UP) != keys.end()) || (std::find(keys.begin(), keys.end(), GLFW_KEY_DOWN) != keys.end())))
        car1->brakeByFriction();
    
    if (std::find(keys.begin(), keys.end(), GLFW_KEY_UP) != keys.end())
        car1->accelerate(true);
        
    if (std::find(keys.begin(), keys.end(), GLFW_KEY_DOWN) != keys.end())
        car1->accelerate(false);
   
    if (std::find(keys.begin(), keys.end(), GLFW_KEY_LEFT) != keys.end())
        car1->turn(false);
   
    if (std::find(keys.begin(), keys.end(), GLFW_KEY_RIGHT) != keys.end())
        car1->turn(true);
    
    if (std::find(keys.begin(), keys.end(), GLFW_KEY_SPACE) != keys.end())
        car1->brake();
}

