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
    
    // single keys
    if(!(std::find(keys.begin(), keys.end(), GLFW_KEY_1) != keys.end())
       && !(std::find(keys.begin(), keys.end(), GLFW_KEY_2) != keys.end())
       && !(std::find(keys.begin(), keys.end(), GLFW_KEY_3) != keys.end())
       && !(std::find(keys.begin(), keys.end(), GLFW_KEY_LEFT_SHIFT) != keys.end())
       && !(std::find(keys.begin(), keys.end(), GLFW_KEY_B) != keys.end())
       ){
        switch (key)
        {
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window, GL_TRUE);
                break;
                
            case GLFW_KEY_UP:
                car1->accelerate(true);
                break;
                
            case GLFW_KEY_DOWN:
                car1->accelerate(false);
                break;
                
            case GLFW_KEY_LEFT:
                car1->turn(false);
                break;
                
            case GLFW_KEY_RIGHT:
                car1->turn(true);
                break;
                
            case GLFW_KEY_SPACE:
                car1->brake();
                break;
                
            default:
                // TODO: dafuer sorgen, dass auch aufgerufen, wenn kein key gedrueckt ist (keycallback nicht aufgerufen)
                car1->brakeByFriction();
                break;
        }
    }
    
    // key combinations
    //
    //    if (std::find(keys.begin(), keys.end(), GLFW_KEY_LEFT_SHIFT) != keys.end()) {
    //        if(std::find(keys.begin(), keys.end(), GLFW_KEY_Z) != keys.end()){
    //            angleZ = angleZ - 0.1f;
    //        }
    //    }
    //
    //    if (std::find(keys.begin(), keys.end(), GLFW_KEY_1) != keys.end()) {
    //        if(std::find(keys.begin(), keys.end(), GLFW_KEY_UP) != keys.end()){
    //            angleZBaseSeg1 = angleZBaseSeg1 + 0.1f;
    //        } else if (std::find(keys.begin(), keys.end(), GLFW_KEY_DOWN) != keys.end()){
    //            angleZBaseSeg1 = angleZBaseSeg1 - 0.1f;
    //        }
    //    }
    //    if (std::find(keys.begin(), keys.end(), GLFW_KEY_2) != keys.end()) {
    //        if(std::find(keys.begin(), keys.end(), GLFW_KEY_UP) != keys.end()){
    //            angleZSeg1Seg2 = angleZSeg1Seg2 + 0.1f;
    //        } else if (std::find(keys.begin(), keys.end(), GLFW_KEY_DOWN) != keys.end()){
    //            angleZSeg1Seg2 = angleZSeg1Seg2 - 0.1f;
    //        }
    //    }
    //    if (std::find(keys.begin(), keys.end(), GLFW_KEY_3) != keys.end()) {
    //        if(std::find(keys.begin(), keys.end(), GLFW_KEY_UP) != keys.end()){
    //            angleZSeg2Seg3 = angleZSeg2Seg3 + 0.1f
    //        } else if (std::find(keys.begin(), keys.end(), GLFW_KEY_DOWN) != keys.end()){
    //            angleZSeg2Seg3 = angleZSeg2Seg3 - 0.1f;
    //        }
    //    }
    //
    //    if (std::find(keys.begin(), keys.end(), GLFW_KEY_B) != keys.end()) {
    //        if(std::find(keys.begin(), keys.end(), GLFW_KEY_LEFT) != keys.end()){
    //            angleYBase = angleYBase + 0.1f;
    //        } else if (std::find(keys.begin(), keys.end(), GLFW_KEY_RIGHT) != keys.end()){
    //            angleYBase = angleYBase - 0.1f;
    //        }
    //    }
    //
}
