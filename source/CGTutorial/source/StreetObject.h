#ifndef PROOF_STREETOBJECT_H
#define PROOF_STREETOBJECT_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;

#include "Model.h"

class StreetObject {
    public:
        StreetObject(Model m): model(m) {
            matrix = glm::mat4(1.0f);
        }
        void setMatrix(glm::mat4 m) {
            this->matrix = m;
        }
        glm::mat4 getMatrix() {
            return this->matrix;
        }

        void setTexture(GLuint textId) {
            this->model.setTexture(textId);
        }

        void draw(Shader shader) {
            this->model.Draw(shader);
        }

    private:
        Model model;
        glm::mat4 matrix;
};

#endif //PROOF_STREETOBJECT_H
