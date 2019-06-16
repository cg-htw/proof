#include "Car.hpp"


Car::Car(Model carModel, float maxVelocity): carModel(carModel), maxVelocity(maxVelocity)
{
//    this->carModel = carModel;
//    this->maxVelocity = maxVelocity;
    carMatrix = glm::mat4(1.0f);
    translation = glm::vec3(0.0f);
    rotation = glm::vec3(0.0f);
    scaleFactor = 1.0f;

    velocity = 0;
    minVelocity = -500.0f;
    positiveAcceleration = 5.0f;
    negativeAcceleration = -2.0f;
    turnAngle = 4.0f;
    brakingForce = 0.2f;
    friction = 0.05f;
}

glm::mat4 Car::getModel(){
    return carMatrix;
}

void Car::setMaxVelocity(float maxVelocity){
    this->maxVelocity = maxVelocity;
}
void Car::setMinVelocity(float minVelocity){
    this->minVelocity = minVelocity;
}

void Car::setPositiveAcceleration(float positiveAcceleration){
    this->positiveAcceleration = positiveAcceleration;
}
void Car::setNegativeAcceleration(float negativeAcceleration){
    this->negativeAcceleration = negativeAcceleration;
}

void Car::setTurnAngle(float turnAngle){
    this->turnAngle = turnAngle;
}

void Car::setBrakingForce(float brakingForce){
    this->brakingForce = brakingForce;
}

void Car::setFriction(float friction){
    this->friction = friction;
}


void Car::moveBy(glm::vec3 deltaTranslation){
    translation += deltaTranslation;
    carMatrix = glm::translate(carMatrix, deltaTranslation);
}

//void Car::moveTo(glm::vec3 translation){
//    this->translation = translation;
//    carMatrix = glm::translate(carMatrix, translation);
//}

void Car::accelerate(bool forward){
    velocity -= forward? positiveAcceleration : negativeAcceleration; // TODO> herausfinden warum hier substraktion notwenidg ist und entsprechend aendern
    if(this->velocity > maxVelocity)
        this->velocity = maxVelocity;
    if(this->velocity < minVelocity)
        this->velocity = minVelocity;
 }

void Car::brake(){
    if (velocity > 0.0) {
        velocity -= brakingForce;
        if (velocity < 0.0)
            velocity = 0.0;
    } else if (velocity < 0.0) {
        velocity += brakingForce;
        if (velocity > 0.0)
            velocity = 0.0;
    }
}

void Car::brakeByFriction(){
    printf("brake bz friction");
    if (velocity > 0.0) {
        velocity -= friction;
        if (velocity < 0.0)
            velocity = 0.0;
    } else if (velocity < 0.0) {
        velocity += friction;
        if (velocity > 0.0)
            velocity = 0.0;
    }
}

void Car::turn(bool right){
    pendingTurn -= right? turnAngle : -turnAngle;
    this->rotation[2] -= pendingTurn;
}

void Car::rotateBy(glm::vec3 deltaRotation){
    this->rotation += deltaRotation;
    carMatrix = glm::rotate(carMatrix, deltaRotation[0], glm::vec3(1.0, 0.0, 0.0) );
    carMatrix = glm::rotate(carMatrix, deltaRotation[1], glm::vec3(0.0, 1.0, 0.0) );
    carMatrix = glm::rotate(carMatrix, deltaRotation[2], glm::vec3(0.0, 0.0, 1.0) );
}

//void Car::rotateTo(glm::vec3 rotation){
//    this->rotation = rotation;
//    carMatrix = glm::rotate(carMatrix, rotation[0], glm::vec3(1.0, 0.0, 0.0) );
//    carMatrix = glm::rotate(carMatrix, rotation[1], glm::vec3(0.0, 1.0, 0.0) );
//    carMatrix = glm::rotate(carMatrix, rotation[2], glm::vec3(0.0, 0.0, 1.0) );
//}


void Car::scale(float scaleFactor){
    this->scaleFactor = scaleFactor;
    carMatrix = glm::scale(carMatrix, glm::vec3(scaleFactor));

}

void Car::setTexture(GLuint textId) {
    carModel.setTexture(textId);
}

void Car::performTransformations(){
    currentTime = glfwGetTime();
    deltaTime = currentTime - lastTime;
    lastTime = currentTime;
    
    rotateBy(glm::vec3(0.0f, 0.0f, pendingTurn * (float) deltaTime));
//    carMatrix = glm::rotate(carMatrix, pendingTurn * (float) deltaTime, glm::vec3(0.0, 0.0, 1.0) );
    pendingTurn = 0.0f;

    moveBy(glm::vec3(0.0f, velocity * deltaTime, 0.0f)); // TODO: change code so that moveBy

}

void Car::draw(Shader shader){
    performTransformations();
    history.push_back({currentTime, carMatrix});

    carModel.Draw(shader);
    
    // evt.update der Vektoren notwendig, aber eig nicht.
    // TODO: carModel.Draw(shader);  // wahrscheinlich sinnvoll ein Attribut für den Pointer auf den Shader anzulegen.
}

