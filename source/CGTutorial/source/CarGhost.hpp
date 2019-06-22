//
//  GhostController.hpp
//  CG_Beleg
//
//  Created by Janis on 18.06.19.
//

#pragma once

#include <stdio.h>

#include "Car.hpp"

class CarGhost
{
    std::vector<HistoryEntry> history;
    Model carModel;
    glm::mat4 *carModelMatrix;
    int currentPosition;
    double previousTimestamp;
    double currentTimestamp;

public:
    CarGhost(const std::string& file, Model carModel); // evt das Model übergeben
    glm::mat4 getModelMatrix();
    void loadHistoryFromFile(const std::string& file);
    void updateCarMatrixWithHistory(double time);
    void draw(Shader shader, double time);
};
