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
    float brakingForce;
    float friction;
    
    
public:
    
    // constructor
    Car(Model carModel, float maxVelocity); // evt das Model übergeben
    // evt. weiterer Konstruktor, bei dem man auch rotation und position übergeben kann, mit denen dann moveTo und rotateTo aufgerufen werden
    
    void moveBy(glm::vec3 deltaTranslation);
    void moveTo(glm::vec3 translation);
    
    void accelerateBy(float acceleration); // called when w/s is pressed. increases velocity until max/min is reached
    void brake();
    void brakeByFriction();
    void setMaxVelocity(float maxVelocity);
    void setMinVelocity(float maxVelocity);
    void setBrakingForce(float brakingForce);
    void setFriction(float friction);
    void turn(float angle);
    void rotateBy(glm::vec3 deltaRotation); // TODO> delete
    void rotateTo(glm::vec3 rotation); // TODO> delete
   
    void scale(float scaleFactor);
    
    glm::mat4 getModel();
    void setTexture(GLuint textId);
    
    void draw(Shader shader);
     // destructor probably not needed
//    ~Car();
};

