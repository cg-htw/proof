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
#include "glm/ext.hpp"

struct historyEntry {
    double time;
    glm::mat4 carMatrix;
};

class Car
{
    // Auslagern in Transform, was bisschen sowas wie ein interface/partial ist und für StreetPart wiederverwendet werden kann
    Model carModel;
    glm::mat4 carMatrix;
    
    glm::vec3 translation;
    glm::vec3 rotation;
    float scaleFactor;
    // glm::vec3 skew;
    // glm::vec4 perspective
    
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
    
    std::vector<historyEntry> history;
    
public:
    
    // constructor
    Car(Model carModel, float maxVelocity); // evt das Model übergeben
    // evt. weiterer Konstruktor, bei dem man auch rotation und position übergeben kann, mit denen dann moveTo und rotateTo aufgerufen werden
    
    void moveBy(glm::vec3 deltaTranslation);
//    void moveTo(glm::vec3 translation);
    
    void accelerate(bool forward); // called when w/s is pressed. increases velocity until max/min is reached
    void brake();
    void brakeByFriction();
    void setMaxVelocity(float maxVelocity);
    void setMinVelocity(float maxVelocity);
    void setPositiveAcceleration(float positiveAcceleration);
    void setNegativeAcceleration(float negativeAcceleration);
    void setTurnAngle(float turnAngle);
    void setBrakingForce(float brakingForce);
    void setFriction(float friction);
    void turn(bool right);
    void rotateBy(glm::vec3 deltaRotation); // TODO> delete
//    void rotateTo(glm::vec3 rotation); // TODO> delete
   
    void scale(float scaleFactor);
    
    glm::mat4 getModel();
    void setTexture(GLuint textId);
    
    void performTransformations();
    void draw(Shader shader);
    
    void saveHistoryToFile(const std::string& file);
    void loadHistoryFromFile(const std::string& file);
     // destructor probably not needed
//    ~Car();
};

