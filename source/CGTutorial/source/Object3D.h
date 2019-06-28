#ifndef PROOF_STREETOBJECT_H
#define PROOF_STREETOBJECT_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;

#include "Model.hpp"

class Object3D {
    public:
        Object3D(Model m): model(m) {
            matrix = glm::mat4(1.0f);
        }
    
        glm::mat4 getMatrix() {
            return this->matrix;
        }

        void translateTo(glm::vec3 translation){
            matrix = glm::translate(matrix, translation);
        }
    
        void rotateTo(glm::vec3 rotation){
            matrix = glm::rotate(matrix, rotation[0], glm::vec3(1.0, 0.0, 0.0) );
            matrix = glm::rotate(matrix, rotation[1], glm::vec3(0.0, 1.0, 0.0) );
            matrix = glm::rotate(matrix, rotation[2], glm::vec3(0.0, 0.0, 1.0) );
        }
    
        void scale(glm::vec3 scaleVector){
            matrix = glm::scale(matrix, scaleVector );
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
