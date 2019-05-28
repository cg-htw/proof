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
#include "Model.h"


// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;

// Other Libs
#include "SOIL2/SOIL2.h"

// Achtung, die OpenGL-Tutorials nutzen glfw 2.7, glfw kommt mit einem ver‰nderten API schon in der Version 3

// Befindet sich bei den OpenGL-Tutorials unter "common"
#include "shader.hpp"

// Wuerfel und Kugel
#include "objects.hpp"

#include "objloader.hpp"

#include "texture.hpp"

// Properties
const GLuint WIDTH = 800, HEIGHT = 600;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// for send_MVP
// Diese Drei Matrizen global (Singleton-Muster), damit sie jederzeit modifiziert und
// an die Grafikkarte geschickt werden koennen
glm::mat4 projection;
glm::mat4 view;
glm::mat4 model;
GLuint programID;

// for key_callback
float angleX = 0.0f;
float angleY = 0.0f;
float angleZ = 0.0f;

float angleYBase = 0.0f;
float angleZBaseSeg1 = 0.0f;
float angleZSeg1Seg2 = 0.0f;
float angleZSeg2Seg3 = 0.0f;

std::vector<int> keys;


// Function prototypes
void error_callback( int error, const char* description );
void key_callback( GLFWwindow* window, int key, int scancode, int action, int mods );
// void mouse_callback( GLFWwindow *window, double xPos, double yPos );
void sendMVP();
void zeichneKS();
void zeichneSeg(float h);
// void DoMovement( );

int main(void)
{
    // Initialise GLFW
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        exit(EXIT_FAILURE);
    }
    
    // Fehler werden auf stderr ausgegeben, s. o.
    glfwSetErrorCallback(error_callback); // Übergeben zeiger auf funktion, die bei fehler ausgeführt wird
    
    
    // Die folgenden vier Zeilen sind nötig auf dem Mac
    // Außerdem müssen die zu ladenden Dateien bei der aktuellen Projektkonfiguration
    // unter DerivedData/Build/Products/Debug (oder dann Release) zu finden sein
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // Open a window and create its OpenGL context
    // glfwWindowHint vorher aufrufen, um erforderliche Resourcen festzulegen
    // wegen * ist window pointer auf das objekt
    GLFWwindow* window = glfwCreateWindow(WIDTH, // Breite
                                          HEIGHT,  // Hoehe
                                          "Car Race", // Ueberschrift
                                          NULL,  // windowed mode
                                          NULL); // shared window
    // ich könnte hier z.b. noch ein weiteres Fenster definieren. Müsste dann zum rendern immer den context zu dem aktuellen window setzen. (glfwMakeContextCurrent(window2); Zumindest in manchen fällen, in anderen kann man bei dem jeweiligen Befehl das Fnester für das es passieren soll angeben
    
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    
    // Make the window's context current (wird nicht automatisch gemacht)
    // Definiert, dass alles was gemalt wird in window gemalt wird. Um in verschiedene fenster zu malen
    // immer diesen Befehl aufrufen mit dem entsprechenden Fenster.
    glfwMakeContextCurrent(window);
    
    // Initialize GLEW
    // GLEW ermˆglicht Zugriff auf OpenGL-API > 1.1
    glewExperimental = true; // Needed for core profile
    
    if (glewInit() != GLEW_OK)
    {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return -1;
    }
    
    // Auf Keyboard-Events reagieren
    glfwSetKeyCallback(window, key_callback); // so kann man unterscheiden für welches fenster man was definieren möchte,...
    
    // Dark blue background
    // RGB werte
    // der 4. wert steht für Allpha: Transparenz/Deckkraft (0 --> man sieht nichts, 1 komplett deckend). zw. 0-1. Bisher wird aber noch keine Transparenzdarstellung unterstüzt
    // die löschfarbe wird gesetzt. die wird dann beim kl.schen, also in glClear verwendet. Sprich dort wird dann das gezeichnete mit der clearColor übermalt
    //
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
    
    // Z-Buffer test aktivieren. (wenn pixel näher an betrachter ist als vorhandener in z-buffer, oder keiner vorhanden, dann malen)
    glEnable(GL_DEPTH_TEST);
    
    // Accept fragment if it closer to the camera than the former one
    // entspricht default, daher eig. nicht notwendig
    glDepthFunc(GL_LESS);
    
    // Create and compile our GLSL program from the shaders
    // sagt aus, dass der Vertex-shader das TVS-vs programm bekommt (mit programm ID, definiert in nächsten befehl
    // und, dass Fragmentshader das CFS.fs bekommt
    // programID = LoadShaders("resources/TransformVertexShader.vertexshader", "resources/ColorFragmentShader.fragmentshader");
    programID = LoadShaders("resources/StandardShading.vertexshader", "resources/StandardShading.fragmentshader");
    
    
    // Shader auch benutzen !
    glUseProgram(programID);
    
    // TODO: Change to relative paths and make it work
    // Setup and compile our shaders
    Shader shader( "/Users/janis/Documents/uni_ss19/CG/proof/source/CGTutorial/source/res/shaders/modelLoading.vs", "/Users/janis/Documents/uni_ss19/CG/proof/source/CGTutorial/source/res/shaders/modelLoading.frag" );
    
    // Load models
//    Model ourModel( "/Users/janis/Documents/uni_ss19/CG/proof/source/CGTutorial/source/res/models/nanosuit.obj" );
    Model ourModel( "resources/Car/Chevrolet_Camaro_SS_1.3ds" );
    //Model ourModel( "resources/Car/Chevrolet_Camaro_SS_5.fbx" );
    
    
    // 2     // damit sind dann daten zu grafikkarte übertragen.
    // lesen daten ein
    // arrays mit variabler länge
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    bool res = loadOBJ("resources/teapot.obj", vertices, uvs, normals);
    // bool res = loadOBJ("resources/Chevrolet_Camaro_SS_High.obj", vertices, uvs, normals);
    // bool res = loadOBJ("resources/Car/Car_Obj.obj", vertices, uvs, normals);
    //bool res = loadOBJ("resources/moskvitch.obj", vertices, uvs, normals);
    
    // Jedes Objekt eigenem VAO zuordnen, damit mehrere Objekte moeglich sind
    // VAOs sind Container fuer mehrere Buffer, die zusammen gesetzt werden sollen.
    // Container anlegen, der VertexArryIDTeapot heißen sol
    GLuint vertexArrayIDTeapot;
    glGenVertexArrays(1, &vertexArrayIDTeapot);
    glBindVertexArray(vertexArrayIDTeapot);
    
    // Eckpunkte anlegen, füllen mit vertices
    // Ein ArrayBuffer speichert Daten zu Eckpunkten (hier xyz bzw. Position)
    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer); // Kennung erhalten
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer); // Daten zur Kennung definieren
    // Buffer zugreifbar f¸r die Shader machen
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
    
    // Erst nach glEnableVertexAttribArray kann DrawArrays auf die Daten zugreifen...
    glEnableVertexAttribArray(0); // siehe layout im vertex shader: location = 0
    glVertexAttribPointer(0,  // location = 0
                          3,  // Datenformat vec3: 3 floats fuer xyz
                          GL_FLOAT,
                          GL_FALSE, // Fixedpoint data normalisieren ?
                          0, // Eckpunkte direkt hintereinander gespeichert
                          (void*) 0); // abweichender Datenanfang ?
    
    
    GLuint uvbuffer; // Hier alles analog f¸r Texturkoordinaten in location == 1 (2 floats u und v!)
    glGenBuffers(1, &uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(1); // siehe layout im vertex shader
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    
    // Load the texture
    GLuint texture = loadBMP_custom("resources/mandrill.bmp"); // texture eine zahl,
    
    
    // Übertragen der normalen vektoren in container
    GLuint normalbuffer; // Hier alles analog f¸r Normalen in location == 2
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
        
        // Projection matrix : 45∞ Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
        projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
        
        // Camera matrix
        view = glm::lookAt(glm::vec3(0,0,-5), // Camera is at (0,0,-5), in World Space
                           glm::vec3(0,0,0),  // and looks at the origin
                           glm::vec3(0,1,0)); // Head is up (set to 0,-1,0 to look upside-down)
        
        // Model matrix : an identity matrix (model will be at the origin)
        // d.h. es gibt eine Variable, die Model heißt, eine 4x4 Matrix ist, der wird das ergebnis von
        // glm::mat4(1.0f), also die einheitsmatrix zugewiesen (1 auf HD, sonst 0)
        model = glm::mat4(1.0f);
        
        // das glm:: wäre an sich nicht notwendig, zeigt aber schön wher die fkt kommt
        model = glm::rotate(model, angleX, glm::vec3( 1.0, 0.0, 0.0));
        model = glm::rotate(model, angleY, glm::vec3( 0.0, 1.0, 0.0));
        model = glm::rotate(model, angleZ, glm::vec3( 0.0, 0.0, 1.0));
        
        glm::mat4 save = model;
        model = glm::translate(model, glm::vec3(1.5, 0.0, 0.0));
        
        // 3
        // Skalieren, da sonst viel zu groß
        model = glm::scale(model, glm::vec3(1.0 / 1000.0, 1.0 / 1000.0, 1.0 / 1000.0));
        
        // Bind our texture in Texture Unit 0
        glActiveTexture(GL_TEXTURE0); // da multiple texturing möglich ist notwendig anzugeben welche grad die gewollte ist für dieses Texture Unit.
        glBindTexture(GL_TEXTURE_2D, texture);
        
        // Set our "myTextureSampler" sampler to user Texture Unit 0
        glUniform1i(glGetUniformLocation(programID, "myTextureSampler"), 0);
        
        
        // Shader mitteilen wo sich das licht befindet.
        // relativ beliebig wo, könnte auch vor schleife sein, wenn sich (wie bisher bei uns) die lampenposition nicht ändert
        // glm::vec3 lightPos = glm::vec3(4,4,-4);
        // glUniform3f(glGetUniformLocation(programID, "LightPosition_worldspace"), lightPos.x, lightPos.y, lightPos.z);
        
        sendMVP();
        
        // 4
        // Daten nehmen und zeichnen
        glBindVertexArray(vertexArrayIDTeapot);
        glDrawArrays(GL_TRIANGLES, 0, (float) vertices.size());
        
        // drawWireCube();
        
        
        // Kugel zeichnen
        model = save; // lade die gespeicherte matrix, zur rotation u
        // Model = glm::scale(Model, glm::vec3(0.5, 0.5, 0.5));
        // sendMVP();
        // drawSphere(10, 10);
        
        zeichneKS();
        
        model = glm::rotate(model, angleYBase, glm::vec3( 0.0, 1.0, 0.0));
        
        model = glm::rotate(model, angleZBaseSeg1, glm::vec3( 0.0, 0.0, 1.0));
        zeichneSeg(0.5);
        model = glm::translate(model, glm::vec3(0.0, 0.5, 0.0));
        model = glm::rotate(model, angleZSeg1Seg2, glm::vec3( 0.0, 0.0, 1.0));
        zeichneSeg(0.4);
        model = glm::translate(model, glm::vec3(0.0, 0.4, 0.0));
        model = glm::rotate(model, angleZSeg2Seg3, glm::vec3( 0.0, 0.0, 1.0));
        zeichneSeg(0.3);
        
        glm::vec4 lpw = model * glm::vec4(0.0, 0.3, 0.0, 1.0); // light position world
        glUniform3f(glGetUniformLocation(programID, "LightPosition_worldspace"), lpw.x / lpw.w, lpw.y / lpw.w, lpw.z / lpw.w); // lpw.w = normalisierungszahl, ist aber bei uns sowieso 1
        
        
        
        // draw Würfel
        model = save;
        model = glm::translate(model, glm::vec3(-1.5, 0.0, 0.0));
        model = glm::scale(model, glm::vec3(1.0 / 5.0, 1.0 / 5.0, 1.0 / 5.0));
        
        // Bind our texture in Texture Unit 0
        glActiveTexture(GL_TEXTURE0); // da multiple texturing möglich ist notwendig anzugeben welche grad die gewollte ist für dieses Texture Unit.
        glBindTexture(GL_TEXTURE_2D, texture);
        
        sendMVP();
        //drawCube();
        
        ourModel.Draw( shader );
        
        // Swap buffers
        glfwSwapBuffers(window);
        
        // Poll for and process events
        // z.b. Maus oder Tastatureingabe überprüfen, wenn ja rufe die funktion key_callback auf, sofern diese vorhanden(Ja), oder mouse_callback,... (checkt ob vorhanden, wenn ja wirds ausgeführt)
        glfwPollEvents();
    }
    
    
    glDeleteProgram(programID);
    
    // 5
    // Cleanup VBO and shader
    // nötig am ende wenn das programm terminiert. Macht aber alternativ auch das bettriebssystem
    glDeleteBuffers(1, &vertexbuffer);
    
    glDeleteBuffers(1, &normalbuffer);
    
    glDeleteBuffers(1, &uvbuffer);
    glDeleteTextures(1, &texture);
    
    
    // Close OpenGL window and terminate GLFW
    glfwTerminate();
    return 0;
}

/*
 Genau diese Parameter für glfwSetErrorCallback(error_callback) nötig
 */
void error_callback(int error, const char* description)
{
    fputs(description, stderr);
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    switch (action) {
        case GLFW_PRESS:
            keys.push_back(key);
            break;
            
        case GLFW_RELEASE:
            // if (std::find(keys.begin(), keys.end(), key) != keys.end()){
            keys.erase(std::remove(keys.begin(), keys.end(), key), keys.end());
            // }
            break;
    }
    
    if(!(std::find(keys.begin(), keys.end(), GLFW_KEY_1) != keys.end())
       && !(std::find(keys.begin(), keys.end(), GLFW_KEY_2) != keys.end())
       && !(std::find(keys.begin(), keys.end(), GLFW_KEY_3) != keys.end())
       && !(std::find(keys.begin(), keys.end(), GLFW_KEY_LEFT_SHIFT) != keys.end())
       && !(std::find(keys.begin(), keys.end(), GLFW_KEY_B) != keys.end())
       ){
        switch (key)
        {
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window, GL_TRUE);
                break;
                
            case GLFW_KEY_UP:
                angleX = angleX + 0.1f;
                
                break;
                
            case GLFW_KEY_DOWN:
                angleX = angleX - 0.1f;
                break;
                
            case GLFW_KEY_LEFT:
                angleY = angleY - 0.1f;
                break;
                
            case GLFW_KEY_RIGHT:
                angleY = angleY + 0.1f;
                break;
                
            case GLFW_KEY_Z:
                angleZ = angleZ + 0.1f;
                break;
                
                /*case GLFW_KEY_Q: // GLFW_KEY_1 & GLFW_KEY_UP:
                 angleZBaseSeg1 = angleZBaseSeg1 - 0.1f;
                 break;
                 
                 case GLFW_KEY_A: // GLFW_KEY_1 & GLFW_KEY_DOWN:
                 angleZBaseSeg1 = angleZBaseSeg1 + 0.1f;
                 break;
                 
                 case GLFW_KEY_W: // GLFW_KEY_2 & GLFW_KEY_UP:
                 angleZSeg1Seg2 = angleZSeg1Seg2 - 0.1f;
                 break;
                 
                 case GLFW_KEY_S: // GLFW_KEY_2 & GLFW_KEY_DOWN:
                 angleZSeg1Seg2 = angleZSeg1Seg2 + 0.1f;
                 break;
                 
                 case GLFW_KEY_E: // GLFW_KEY_3 & GLFW_KEY_UP:
                 angleZSeg2Seg3 = angleZSeg2Seg3 - 0.1f;
                 break;
                 
                 case GLFW_KEY_D: // GLFW_KEY_4 & GLFW_KEY_DOWN:
                 angleZSeg2Seg3 = angleZSeg2Seg3 + 0.1f;
                 break;
                 
                 case GLFW_KEY_R:
                 angleYBase = angleYBase - 0.1f;
                 break;
                 
                 case GLFW_KEY_L:
                 angleYBase = angleYBase + 0.1f;
                 break;
                 */
            default:
                break;
        }
    }
    
    if (std::find(keys.begin(), keys.end(), GLFW_KEY_LEFT_SHIFT) != keys.end()) {
        if(std::find(keys.begin(), keys.end(), GLFW_KEY_Z) != keys.end()){
            angleZ = angleZ - 0.1f;
        }
    }
    
    if (std::find(keys.begin(), keys.end(), GLFW_KEY_1) != keys.end()) {
        if(std::find(keys.begin(), keys.end(), GLFW_KEY_UP) != keys.end()){
            angleZBaseSeg1 = angleZBaseSeg1 + 0.1f;
        } else if (std::find(keys.begin(), keys.end(), GLFW_KEY_DOWN) != keys.end()){
            angleZBaseSeg1 = angleZBaseSeg1 - 0.1f;
        }
    }
    if (std::find(keys.begin(), keys.end(), GLFW_KEY_2) != keys.end()) {
        if(std::find(keys.begin(), keys.end(), GLFW_KEY_UP) != keys.end()){
            angleZSeg1Seg2 = angleZSeg1Seg2 + 0.1f;
        } else if (std::find(keys.begin(), keys.end(), GLFW_KEY_DOWN) != keys.end()){
            angleZSeg1Seg2 = angleZSeg1Seg2 - 0.1f;
        }
    }
    if (std::find(keys.begin(), keys.end(), GLFW_KEY_3) != keys.end()) {
        if(std::find(keys.begin(), keys.end(), GLFW_KEY_UP) != keys.end()){
            angleZSeg2Seg3 = angleZSeg2Seg3 + 0.1f;
        } else if (std::find(keys.begin(), keys.end(), GLFW_KEY_DOWN) != keys.end()){
            angleZSeg2Seg3 = angleZSeg2Seg3 - 0.1f;
        }
    }
    
    if (std::find(keys.begin(), keys.end(), GLFW_KEY_B) != keys.end()) {
        if(std::find(keys.begin(), keys.end(), GLFW_KEY_LEFT) != keys.end()){
            angleYBase = angleYBase + 0.1f;
        } else if (std::find(keys.begin(), keys.end(), GLFW_KEY_RIGHT) != keys.end()){
            angleYBase = angleYBase - 0.1f;
        }
    }
    
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

void zeichneKS()
{
    glm::mat4 Save = model;
    model = glm::scale(Save, glm::vec3(5.0,0.01,0.01));
    sendMVP();
    drawCube();
    model=Save;
    
    Save = model;
    model = glm::scale(Save, glm::vec3(0.01, 5.0, 0.01));
    sendMVP();
    drawCube();
    model=Save;
    
    Save = model;
    model = glm::scale(Save, glm::vec3(0.01, 0.01, 5.0));
    sendMVP();
    drawCube();
    model=Save;
}

void zeichneSeg(float h)
{
    glm::mat4 Save = model;
    model = glm::translate(model, glm::vec3(0.0, h / 2, 0.0)); // wie in schwarzer zeichnung (leserichtung aufwärts)
    model = glm::scale(model, glm::vec3(h / 6.0, h / 2, h / 6.0 ));
    //Model = glm::translate(Model, glm::vec3(0.0, 1.0, 0.0)); // alternativlösung, wie in blauer zeichnung (leserichtung aufwärts)
    
    sendMVP();
    // radius = 1; bezugskoordinatensystem: mittelpunkt des koordinatensystems im mittelpunkt
    drawSphere(10.0, 10.0);
    model=Save;
}
