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

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;

// Other Libs
#include "SOIL2/SOIL2.h"

// fromn OpenGL Tutorial
#include "shader.hpp"

// Wuerfel und Kugel
#include "objects.hpp"

#include "objloader.hpp"

#include "texture.hpp"

#include "text2D.hpp"


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

    Model carModel( "resources/Car/Chevrolet_Camaro_SS_bkp3.3ds" );
    streetCurveA90Model = new Model( "resources/Street_and_landscape/StreetCurveA90.fbx" );
    streetCurveB90Model = new Model( "resources/Street_and_landscape/StreetCurveB90.fbx" );
    streetStraight = new Model( "resources/Street_and_landscape/StreetStraight.fbx" );

    buildLevel1();
    
    
    car1 = new Car(carModel, 10.0f);
    car1->scale(1.0/7.5);
    car1->rotateBy(glm::vec3(glm::radians(-90.0), 0.0, glm::radians(90.0)));
    car1->moveBy(glm::vec3(-3.0, 2.0, 6.0)); // TODO: nach hinten verschieben (negativert z wert), führt bisher zum verschwinden
    
    
    // read data to be passed to graphics card later
    // vectors are basically arrays with variable lenght
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    // bool res = loadOBJ("resources/teapot.obj", vertices, uvs, normals);

    // Assign each Object to its own VAO in order to be able to have multiple objects.
    // VAOs (Vertex Array Objects) are Containers for multiple buffers to be set together.
    GLuint vertexArrayIDTeapot; // Create container
    glGenVertexArrays(1, &vertexArrayIDTeapot);
    glBindVertexArray(vertexArrayIDTeapot);
    
    // Eckpunkte anlegen, fuellen mit vertices
    // Ein ArrayBuffer speichert Daten zu Eckpunkten (hier xyz bzw. Position)
    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer); // Kennung erhalten
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer); // Daten zur Kennung definieren
    // Buffer zugreifbar f�r die Shader machen
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
    
    // Erst nach glEnableVertexAttribArray kann DrawArrays auf die Daten zugreifen...
    glEnableVertexAttribArray(0); // siehe layout im vertex shader: location = 0
    glVertexAttribPointer(0,  // location = 0
                          3,  // Datenformat vec3: 3 floats fuer xyz
                          GL_FLOAT,
                          GL_FALSE, // Fixedpoint data normalisieren ?
                          0, // Eckpunkte direkt hintereinander gespeichert
                          (void*) 0); // abweichender Datenanfang ?
    

    GLuint uvbuffer; // Hier alles analog fuer Texturkoordinaten in location == 1 (2 floats u und v!)
    glGenBuffers(1, &uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(1); // siehe layout im vertex shader
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // Load the monkey_texture
//    GLuint monkey_texture = loadBMP_custom("resources/red.bmp"); // monkey_texture eine zahl,
     GLuint monkey_texture = loadBMP_custom("resources/mandrill.bmp"); // monkey_texture eine zahl,

    street_texture = TextureFromFile("rua.jpg", "resources/Street_and_landscape");
    grass_texture = TextureFromFile("grass.jpg", "resources/Street_and_landscape");
    
    initText2D( "resources/Text2D/Holstein.DDS" );
    
    
    
    // �bertragen der normalen vektoren in container
    GLuint normalbuffer; // Hier alles analog f�r Normalen in location == 2
    glGenBuffers(1, &normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(2); // siehe layout im vertex shader
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    
    // Eventloop
    while (!glfwWindowShouldClose(window))
    {
        // Clear the screen (depth of each frame, color)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Projection matrix : 45� Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
        projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);

        // Camera matrix
        view = glm::lookAt(glm::vec3(0,20,-15), // Camera is at (0,2,-5), in World Space
                           glm::vec3(0,0,0),  // and looks at the origin
                           glm::vec3(0,1,0)); // Head is up (set to 0,-1,0 to look upside-down

        view = glm::rotate(view, -1.5708f, glm::vec3( 0.0, 1.0, 0.0));
        //
        // Model matrix : an identity matrix (model will be at the origin)
        model = glm::mat4(1.0f);
        


        glm::mat4 save = model;
        model = glm::translate(model, glm::vec3(1.5, 0.0, 0.0));
        

        model = glm::scale(model, glm::vec3(1.0 / 1000.0, 1.0 / 1000.0, 1.0 / 1000.0));

        // Bind our monkey_texture in Texture Unit 0
        glActiveTexture(GL_TEXTURE0); // because of the option to use multiple texturing it ios required to define which monkey_texture to be active.
        glBindTexture(GL_TEXTURE_2D, monkey_texture);
        
        // Set our "myTextureSampler" sampler to user Texture Unit 0
        glUniform1i(glGetUniformLocation(programID, "myTextureSampler"), 0);
        
        
        // Shader mitteilen wo sich das licht befindet.
        // relativ beliebig wo, k�nnte auch vor schleife sein, wenn sich (wie bisher bei uns) die lampenposition nicht �ndert
         glm::vec3 lightPos = glm::vec3(4,15,-4);
         glUniform3f(glGetUniformLocation(programID, "LightPosition_worldspace"), lightPos.x, lightPos.y, lightPos.z);
        
        sendMVP();
    

        // Licht am roboterarm
        //glm::vec4 lpw = model * glm::vec4(0.0, 0.3, 0.0, 1.0); // light position world
        //glUniform3f(glGetUniformLocation(programID, "LightPosition_worldspace"), lpw.x / lpw.w, lpw.y / lpw.w, lpw.z / lpw.w); // lpw.w = normalisierungszahl, ist aber bei uns sowieso 1
        
        
        model = save;
        // Bind our monkey_texture in Texture Unit 0
        //glActiveTexture(GL_TEXTURE0); // da multiple texturing m�glich ist notwendig anzugeben welche grad die gewollte ist f�r dieses Texture Unit.
        //glBindTexture(GL_TEXTURE_2D, monkey_texture);

        model = car1->getModel();

        sendMVP();
        //drawCube();
        car1->draw( *shader );

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
        //printText2D(text.data(), 10, 500, 60);

        
        // Swap buffers
        glfwSwapBuffers(window);
        
        // Poll for and process events
        // z.b. Maus oder Tastatureingabe �berpr�fen, wenn ja rufe die funktion key_callback auf, sofern diese vorhanden(Ja), oder mouse_callback,... (checkt ob vorhanden, wenn ja wirds ausgef�hrt)
        glfwPollEvents();
    }

    
    glDeleteProgram(programID);

    // 5
    // Cleanup VBO and shader
    // n�tig am ende wenn das programm terminiert. Macht aber alternativ auch das bettriebssystem
    glDeleteBuffers(1, &vertexbuffer);
    
    glDeleteBuffers(1, &normalbuffer);
    
    glDeleteBuffers(1, &uvbuffer);
    glDeleteTextures(1, &monkey_texture);
    
    cleanupText2D();
    
    // Close OpenGL window and terminate GLFW
    glfwTerminate();
    return 0;
}


/*
 Genau diese Parameter f�r glfwSetErrorCallback(error_callback) n�tig
 */
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

// TODO: Eine klasse car machen. Die alle Methoden hat die ich zu kontrolle des cars benötige.
/*
 this->spaceship->transform.moveby(vektor, OBJECT) // im object space, hat er variabel geschrieben
 // moveBy, rotateTo, rotateBy  (, moveTo)
 
 in moveby
 translateMatrix
 - transform entähllt alle daten über räumliche informationen meines obejcts( hat u.a. vektoren für oposition , rotation, scale
 if( // erstmal matrix updaten--> neue matrix, die erst an poisiton geschoben, dann rotiert, dann evt. noch skaliert wird.
 dann matrix um die translateMatrix(vector3)
 --> glm::translate(this->transmMatrix, vector3)
 danach updateVectorsFromMatrix(); // um qzu speichern wo matrix wie nach translation ist
 glm::dcompose(this->transMatrix, scale, rot, pos, skew, persp); (jew. davor als vec definieren)
 rot ist ein quaternion, das umrechnen in euler winkel mit glm::degrees(glm::eulerAmgles(rot))
 
*/


void buildLevel1(){
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


void drawLevel1(){
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