// Include standard headers, Reihenfolge relevant
#include <stdio.h>
#include <stdlib.h>
#include <vector> // dass man c++ variante von arrays verwenden kann

// Include GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

// GL includes
#include "Shader.h"
#include "Camera.h"
#include "Model.hpp"

#include "Object3D.h"
#include "Car.hpp"
#include "CarGhost.hpp"

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>

using namespace glm;

// Other Libs
#include "SOIL2/SOIL2.h"

// fromn OpenGL Tutorial
#include "shader.hpp"


#include "objloader.hpp"

#include "texture.hpp"

#include "text2D.hpp"

// TODO remove unused imports


// Properties
const GLuint WIDTH = 1400, HEIGHT = 1400;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// for send_MVP
// Diese Drei Matrizen global (Singleton-Muster), damit sie jederzeit modifiziert und
// an die Grafikkarte geschickt werden koennen
glm::mat4 projection;
glm::mat4 view;
glm::mat4 model;
GLuint programID;
Shader *shader;

Car *car1;
//Car *car2;
Model *carModel;
CarGhost *carGhost = NULL;
#include "Input.hpp"

Model *streetCurveA90Model;
Model *streetCurveB90Model;
Model *streetStraight;
GLint street_texture;
GLint grass_texture;
std::vector<Object3D> streetObjects;
Model *landscape;

// Function prototypes
void error_callback( int error, const char* description );
void key_callback( GLFWwindow* window, int key, int scancode, int action, int mods );
// void mouse_callback( GLFWwindow *window, double xPos, double yPos );
void sendMVP();
void buildLevel1();
void drawLevel1();

int main()
{
    
    // Initialise GLFW
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        exit(EXIT_FAILURE);
    }
    
    // Fehler werden auf stderr ausgegeben, s. o.
    glfwSetErrorCallback(error_callback); // �bergeben zeiger auf funktion, die bei fehler ausgef�hrt wird
    
    // Mac specific. Defines files to be present in DerivedData/Build/Products/Debug (or Release)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // Open a window and create its OpenGL context
    GLFWwindow* window = glfwCreateWindow(WIDTH, // width
                                          HEIGHT,  // height
                                          "Car Race", // Caption
                                          NULL,  // windowed mode
                                          NULL); // shared window
    
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    
    // Make window's context the current context (not done automatically)
    glfwMakeContextCurrent(window);
    
    // Initialize GLEW (to acess OpenGL-API > 1.1)
    glewExperimental = true; // Needed for core profile
    
    if (glewInit() != GLEW_OK)
    {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return -1;
    }
    
    // define wich key_callback function is to be used for the passed window
    glfwSetKeyCallback(window, key_callback);
    
    // Set color to be used when clearing with rgba. Acts as background color
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
    
    // Enable z-buffer test (only paint pixel if there is no other pixel between it and the viewer)
    glEnable(GL_DEPTH_TEST);
    
    // Accept fragment if it closer to the camera than the former one. (GL_LESS is defeaul, thus not required here)
    glDepthFunc(GL_LESS);
    
    
    // Create and compile our GLSL program from the passed shaders
    // programID = LoadShaders("resources/TransformVertexShader.vertexshader", "resources/ColorFragmentShader.fragmentshader");
    programID = LoadShaders("resources/StandardShading.vertexshader", "resources/StandardShading.fragmentshader");
    
    // Use the shader
    glUseProgram(programID);

    // Setup and compile our shaders
    shader = new Shader( "./source/res/shaders/modelLoading.vs", "./source/res/shaders/modelLoading.frag" );
    //shader.Use();
    
    // Load models

    carModel = new Model( "resources/Car/Chevrolet_Camaro_SS_bkp3.3ds" );
    streetCurveA90Model = new Model( "resources/Street_and_landscape/StreetCurveA90.fbx" );
    streetCurveB90Model = new Model( "resources/Street_and_landscape/StreetCurveB90.fbx" );
    streetStraight = new Model( "resources/Street_and_landscape/StreetStraight.fbx" );

    buildLevel1();
    
    
    car1 = new Car(*carModel, 2000.0f);
    car1->scale(1.0/13.5);
    car1->rotateBy(glm::vec3(glm::radians(-90.0), 0.0, glm::radians(90.0)));
    car1->moveBy(glm::vec3(-3.0, 2.0, 2.0)); // TODO: nach hinten verschieben (negativert z wert), führt bisher zum verschwinden
    
    // TODO extraxt nesecary methods from car class to a super class and set up inheritance
//    car2 = new Car(carModel, 10.0f);
//    car2->scale(1.0/13.5);
//    car2->rotateBy(glm::vec3(glm::radians(-90.0), 0.0, glm::radians(90.0)));
//    car2->moveBy(glm::vec3(-3.0, 2.0, 6.0)); // TODO: nach hinten verschieben (negativert z wert), führt bisher zum verschwinden
    
    carGhost = new CarGhost("lastRide.txt", *carModel);

    street_texture = TextureFromFile("rua.jpg", "resources/Street_and_landscape");
    grass_texture = TextureFromFile("grass.jpg", "resources/Street_and_landscape");
    
    initText2D( "resources/Text2D/Holstein.DDS" );
    
    
    // Eventloop
    while (!glfwWindowShouldClose(window))
    {
        // Clear the screen (depth of each frame, color)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Projection matrix : 45� Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
        projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);

        // Model matrix : an identity matrix (model will be at the origin)
        model = glm::mat4(1.0f);
        
        // View is set before last sendMVP()

        glm::mat4 save = model;
        model = glm::translate(model, glm::vec3(1.5, 0.0, 0.0));

        // Set our "myTextureSampler" sampler to user Texture Unit 0
        // TODO: vlt. notwendig mit shader statt programmID fuer die Zeitanzeige
        glUniform1i(glGetUniformLocation(programID, "myTextureSampler"), 0);
        
         glm::vec3 lightPos = glm::vec3(4,15,-4);
         glUniform3f(glGetUniformLocation(programID, "LightPosition_worldspace"), lightPos.x, lightPos.y, lightPos.z);
        
        sendMVP();
        
        model = save;
        // Bind our monkey_texture in Texture Unit 0
        //glActiveTexture(GL_TEXTURE0); // because of the option to use multiple texturing it ios required to define which monkey_texture to be active.
        //glBindTexture(GL_TEXTURE_2D, monkey_texture);


        // TODO: wahrscheinlich schlauer ausserhalb der schleife, da nur einmal yu yeichnen
        drawLevel1();
        
        
        //get time since game started
        GLint64 timer;
        glGetInteger64v(GL_TIMESTAMP, &timer);
        timer = timer/1000000000.0;
        //display time in window
        std::string text = std::to_string(timer);
        glColor3f(1, 1, 1);
        //printf("Seconds: %s\n", text.data());

        // TODO: make it work
//        printText2D(text.data(), 10, 500, 60);

        
        model = car1->getModelMatrix();
    
        sendMVP();
        //drawCube();
        car1->draw( *shader );
        
        model = carGhost->getModelMatrix();
        sendMVP();
        // TODO: anderes Model fuer ghost --> min. andere farbe, besser noch transparent
        carGhost->draw(*shader, glfwGetTime());
        
        
        glm::vec3 scale;
        glm::quat rotationQuat;
        glm::vec3 translation;
        glm::vec3 skew;
        glm::vec4 perspective;
        glm::decompose(car1->getModelMatrix(), scale, rotationQuat, translation, skew, perspective);
        
        glm::mat3 rotationMatrix = glm::toMat3(rotationQuat);
        glm::vec3 direction = rotationMatrix * glm::vec3(0.0f,1.0f,0.0f);
        glm::vec3 from = translation - direction * -0.6f;
        from[1] = from[1] + 0.3;
        
        // Set camera matrix
        view = glm::lookAt(from, // Camera is at (0,2,-5), in World Space
                           glm::vec3(translation.x ,translation.y,translation.z),  // and looks at the origin
                           glm::vec3(0, 1, 0 )); // Head is up (set to 0,-1,0 to look upside-down
        sendMVP();

        // Swap buffers
        glfwSwapBuffers(window);
        
        // Poll for and process events
        // z.b. Maus oder Tastatureingabe �berpr�fen, wenn ja rufe die funktion key_callback auf, sofern diese vorhanden(Ja), oder mouse_callback,... (checkt ob vorhanden, wenn ja wirds ausgef�hrt)
        glfwPollEvents();
        reactOnPressedKeys();
    }

    car1->saveHistoryToFile("lastRide.txt");
    
    glDeleteProgram(programID);

    cleanupText2D();
    
    // Close OpenGL window and terminate GLFW
    glfwTerminate();
    return 0;
}

void error_callback(int error, const char* description)
{
    fputs(description, stderr);
}

// Berechnungen von CPU an GPU schicken
void sendMVP()
{
    // Our ModelViewProjection : multiplication of our 3 matrices
    glm::mat4 MVP = projection * view * model; // das * macht matrizenmultiplikation bei mat4 objekten
    // Send our transformation to the currently bound shader,
    // in the "MVP" uniform, konstant fuer alle Eckpunkte
    glUniformMatrix4fv(glGetUniformLocation(programID, "MVP"), 1, GL_FALSE, &MVP[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(programID, "M"), 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(programID, "V"), 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(programID, "P"), 1, GL_FALSE, &projection[0][0]);
}

void buildLevel1()
{
    landscape = new Model( "resources/Street_and_landscape/landscape.fbx" );
    
    // TODO auslagern in Methode Load-Level1
    Object3D streetObjectStraight(*streetStraight);
    streetObjects.push_back(streetObjectStraight);
    
    // A ist anschluss an gerade
    Object3D streetObjectCurveA90(*streetCurveA90Model);
    streetObjects.push_back(streetObjectCurveA90);
    streetObjects[1].translateTo(glm::vec3(5.0, 0.0, 0.0));
    
    // B ist anschluss an Kurve
    Object3D streetObjectCurveB90(*streetCurveB90Model);
    streetObjects.push_back(streetObjectCurveB90);
    streetObjects[2].translateTo(glm::vec3(10.0, 0.0, 5.0));
    
    streetObjects.push_back(streetObjectStraight);
    streetObjects[3].translateTo(glm::vec3(0.0, 0.0, 10.0));
    
    streetObjects.push_back(streetObjectCurveA90);
    streetObjects[4].rotateTo(glm::vec3(0.0, glm::radians(-180.0), 0.0));
    streetObjects[4].translateTo(glm::vec3(5.0, 0.0, -10.0));
    
    streetObjects.push_back(streetObjectCurveB90);
    streetObjects[5].rotateTo(glm::vec3(0.0, glm::radians(-180.0), 0.0));
    streetObjects[5].translateTo(glm::vec3(10.0, 0.0, -5.0));
}

void drawLevel1()
{
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0, -0.01, 0.0));
    sendMVP();
    landscape->setTexture(grass_texture);
    landscape->Draw(*shader);
    
    //Straße
    model = streetObjects[0].getMatrix();
    sendMVP();
    streetObjects[0].setTexture(street_texture);
    streetObjects[0].draw(*shader);
    
    model = streetObjects[1].getMatrix();
    sendMVP();
    streetObjects[1].setTexture(street_texture);
    streetObjects[1].draw(*shader);
    
    model = streetObjects[2].getMatrix();
    sendMVP();
    streetObjects[2].setTexture(street_texture);
    streetObjects[2].draw(*shader);
    
    model = streetObjects[3].getMatrix();
    sendMVP();
    streetObjects[3].setTexture(street_texture);
    streetObjects[3].draw(*shader);
    
    model = streetObjects[4].getMatrix();
    sendMVP();
    streetObjects[4].setTexture(street_texture);
    streetObjects[4].draw(*shader);
    
    model = streetObjects[5].getMatrix();
    sendMVP();
    streetObjects[5].setTexture(street_texture);
    streetObjects[5].draw(*shader);
}
