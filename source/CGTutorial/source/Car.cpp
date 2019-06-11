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
    minVelocity = -5.0f;
    brakingForce = 1.0f;


}

void Car::moveBy(glm::vec3 deltaTranslation){
    translation += deltaTranslation;
    carMatrix = glm::translate(carMatrix, translation);
}

void Car::moveTo(glm::vec3 translation){
    this->translation = translation;
    carMatrix = glm::translate(carMatrix, translation);
}

void Car::accelerateBy(float acceleration){
    velocity += acceleration;
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

void Car::setMaxVelocity(float maxVelocity){
    this->maxVelocity = maxVelocity;
}
void Car::setMinVelocity(float minVelocity){
    this->minVelocity = minVelocity;
}

void Car::setBrakingForce(float brakingForce){
    this->brakingForce = brakingForce;
}


void Car::rotateBy(glm::vec3 deltaRotation){
    this->rotation += deltaRotation;
    carMatrix = glm::rotate(carMatrix, rotation[0], glm::vec3(1.0, 0.0, 0.0) );
    carMatrix = glm::rotate(carMatrix, rotation[1], glm::vec3(0.0, 1.0, 0.0) );
    carMatrix = glm::rotate(carMatrix, rotation[2], glm::vec3(0.0, 0.0, 1.0) );
}

void Car::rotateTo(glm::vec3 rotation){
    this->rotation = rotation;
    carMatrix = glm::rotate(carMatrix, rotation[0], glm::vec3(1.0, 0.0, 0.0) );
    carMatrix = glm::rotate(carMatrix, rotation[1], glm::vec3(0.0, 1.0, 0.0) );
    carMatrix = glm::rotate(carMatrix, rotation[2], glm::vec3(0.0, 0.0, 1.0) );
}


void Car::scale(float scaleFactor){
    this->scaleFactor = scaleFactor;
    carMatrix = glm::scale(carMatrix, glm::vec3(scaleFactor));

}

glm::mat4 Car::getModel(){
    return carMatrix;
}

void Car::setTexture(GLuint textId) {
    carModel.setTexture(textId);
}

void Car::draw(Shader shader){
//    performTransformations();
    carModel.Draw(shader);
    
    // evt.update der Vektoren notwendig, aber eig nicht.
    // TODO: carModel.Draw(shader);  // wahrscheinlich sinnvoll ein Attribut für den Pointer auf den Shader anzulegen.
}

