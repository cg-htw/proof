#include "Car.hpp"


Car::Car(Model carModel, float maxVelocity): carModel(carModel), maxVelocity(maxVelocity)
{
    carModelMatrix = glm::mat4(1.0f);
    scaleFactor = 1.0f;

    velocity = 0;
    minVelocity = -50.0f;
    positiveAcceleration = 10.0f;
    negativeAcceleration = -5.0f;
    turnAngle = 2.0f;
    brakingForce = 15.0f;
    friction = 5.0f;
}

glm::mat4 Car::getModelMatrix()
{
    return carModelMatrix;
}

void Car::scale(float scaleFactor)
{
    this->scaleFactor = scaleFactor;
    carModelMatrix = glm::scale(carModelMatrix, glm::vec3(scaleFactor));
}

void Car::setTexture(GLuint textId)
{
    carModel.setTexture(textId);
}


void Car::setMaxVelocity(float maxVelocity)
{
    this->maxVelocity = maxVelocity;
}

void Car::setMinVelocity(float minVelocity)
{
    this->minVelocity = minVelocity;
}

void Car::setPositiveAcceleration(float positiveAcceleration)
{
    this->positiveAcceleration = positiveAcceleration;
}

void Car::setNegativeAcceleration(float negativeAcceleration)
{
    this->negativeAcceleration = negativeAcceleration;
}

void Car::setTurnAngle(float turnAngle)
{
    this->turnAngle = turnAngle;
}

void Car::setBrakingForce(float brakingForce)
{
    this->brakingForce = brakingForce;
}

void Car::setFriction(float friction)
{
    this->friction = friction;
}


void Car::moveBy(glm::vec3 deltaTranslation)
{
    carModelMatrix = glm::translate(carModelMatrix, deltaTranslation);
}

//void Car::moveTo(glm::vec3 translation){
//    carMatrix = glm::translate(carMatrix, translation);
//}

void Car::accelerate(bool forward)
{
    velocity += forward ?
        positiveAcceleration * deltaTime
        : negativeAcceleration * deltaTime; // TODO> herausfinden warum hier substraktion notwenidg ist und entsprechend aendern
    if(this->velocity > maxVelocity)
        this->velocity = maxVelocity;
    if(this->velocity < minVelocity)
        this->velocity = minVelocity;
 }

void Car::brake()
{
    if (velocity > 0.0) {
        velocity -= brakingForce * deltaTime;
        if (velocity < 0.0)
            velocity = 0.0;
    } else if (velocity < 0.0) {
        velocity += brakingForce * deltaTime;
        if (velocity > 0.0)
            velocity = 0.0;
    }
}

void Car::brakeByFriction()
{
    if (velocity > 0.0) {
        velocity -= friction * deltaTime;
        if (velocity < 0.0)
            velocity = 0.0;
    } else if (velocity < 0.0) {
        velocity += friction * deltaTime;
        if (velocity > 0.0)
            velocity = 0.0;
    }
}

void Car::turn(bool right)
{
    pendingTurn += right?
        turnAngle * (float) deltaTime
        : -turnAngle * (float) deltaTime;
}

void Car::rotateBy(glm::vec3 deltaRotation)
{
    carModelMatrix = glm::rotate(carModelMatrix, deltaRotation[0], glm::vec3(1.0, 0.0, 0.0) );
    carModelMatrix = glm::rotate(carModelMatrix, deltaRotation[1], glm::vec3(0.0, 1.0, 0.0) );
    carModelMatrix = glm::rotate(carModelMatrix, deltaRotation[2], glm::vec3(0.0, 0.0, 1.0) );
}

//void Car::rotateTo(glm::vec3 rotation){
//    this->rotation = rotation;
//    carMatrix = glm::rotate(carMatrix, rotation[0], glm::vec3(1.0, 0.0, 0.0) );
//    carMatrix = glm::rotate(carMatrix, rotation[1], glm::vec3(0.0, 1.0, 0.0) );
//    carMatrix = glm::rotate(carMatrix, rotation[2], glm::vec3(0.0, 0.0, 1.0) );
//}


void Car::updateTime()
{
    currentTime = glfwGetTime();
    deltaTime = currentTime - lastTime;
    lastTime = currentTime;
}

void Car::performTransformations()
{
    rotateBy(glm::vec3(0.0f, 0.0f, -pendingTurn));
//  carMatrix = glm::rotate(carMatrix, -pendingTurn, glm::vec3(0.0, 0.0, 1.0) );
    pendingTurn = 0.0f;

    moveBy(glm::vec3(0.0f, -velocity * deltaTime, 0.0f)); // TODO: change code so that moveBy
}

void Car::draw(Shader shader)
{
    updateTime();
    performTransformations();
    history.push_back({currentTime, carModelMatrix});

    carModel.Draw(shader);
    
    // evt.update der Vektoren notwendig, aber eig nicht.
    // TODO: carModel.Draw(shader);  // wahrscheinlich sinnvoll ein Attribut für den Pointer auf den Shader anzulegen.
}

// TODO: Auslagern
// TODO: nicht jeden frame speichern
void Car::saveHistoryToFile(const std::string& file)
{
    ofstream fOut (file);
    // equivalent to: ofstream fOut; fOut.open (file);
    
    if (fOut.is_open()) {
        /* ok, proceed with output */
        printf("Saving to: ");
        system("pwd");
        printf("\n");
        for(int i = 0; i < history.size(); i++){
            fOut << history.at(i).time;
            fOut << " : ";
            glm::vec3 abc(1.0f);
            std::string matrix = to_string(history.at(i).carMatrix).erase(0, 6);
            // TODO: make it work
            fOut << matrix << std::endl;
            fOut << "\n";
        }
        fOut.close();
    } else cout << "Unable to open file";
}

