//
//  Car.hpp
//  CG_Beleg
//
//  Created by Janis on 08.06.19.
//

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

#include "Model.hpp"

// FileIO
#include <iostream>
#include <fstream>
#include <iterator>
//#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>

struct HistoryEntry {
    double time;
    glm::mat4 carMatrix;
};

class Car
{
    // Auslagern in Transform, was bisschen sowas wie ein interface/partial ist und für StreetPart wiederverwendet werden kann
    Model carModel;
    glm::mat4 carModelMatrix;
    float scaleFactor;
    
    float velocity;
    float maxVelocity;
    float minVelocity;
    float positiveAcceleration;
    float negativeAcceleration;
    float turnAngle;
    float pendingTurn;
    float brakingForce;
    float friction;
    
    double lastTime;
    double deltaTime;
    double currentTime;
    
    std::vector<HistoryEntry> history;
    
public:
    
    // constructor
    Car(Model carModel, float maxVelocity);
  
    glm::mat4 getModelMatrix();
    void scale(float scaleFactor);
    void setTexture(GLuint textId);
    
    void setMaxVelocity(float maxVelocity);
    void setMinVelocity(float maxVelocity);
    void setPositiveAcceleration(float positiveAcceleration);
    void setNegativeAcceleration(float negativeAcceleration);
    void setTurnAngle(float turnAngle);
    void setBrakingForce(float brakingForce);
    void setFriction(float friction);
    
    void moveBy(glm::vec3 deltaTranslation);
    //    void moveTo(glm::vec3 translation);
    void accelerate(bool forward); // called when w/s is pressed. increases velocity until max/min is reached
    void brake();
    void brakeByFriction();
    void turn(bool right);
    void rotateBy(glm::vec3 deltaRotation); // TODO> delete
//    void rotateTo(glm::vec3 rotation); // TODO> delete
   
    void updateTime();
    void performTransformations();
    void draw(Shader shader);
    void saveHistoryToFile(const std::string& file, float timestamps[], int sizeTimestamps);
    
     // destructor probably not needed
//    ~Car();
};

