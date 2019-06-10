// Include standard headers, Reihenfolge relevant
#include <stdio.h>
#include <stdlib.h>
#include <vector> // dass man c++ variante von arrays verwenden kann

// Include GLEW
#define GLEW_STATIC
#include <GL/glew.h>

//include Glut
#include <GL/freeglut.h>

// Include GLFW
#include <GLFW/glfw3.h>

// GL includes
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "StreetObject.h"

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

// Properties
const GLuint WIDTH = 1000, HEIGHT = 1400;
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

float movement = 0.0f;
float speed = 0.5f;
float speedBackwards = 2.0f;
float turn = 0; // gradmaß, Drehung
std::vector<int> keys;


// Function prototypes
void error_callback( int error, const char* description );
void key_callback( GLFWwindow* window, int key, int scancode, int action, int mods );
// void mouse_callback( GLFWwindow *window, double xPos, double yPos );
void sendMVP();
void zeichneKS();
void zeichneSeg(float h);
void drawText(const char *text, int length, int x, int y);
// void DoMovement( );
glm::mat4 drawCar(glm::mat4 model);

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);

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
    Shader shader( "./source/res/shaders/modelLoading.vs", "./source/res/shaders/modelLoading.frag" );
    //shader.Use();
    
    // Load models
    //    Model ourModel( "/Users/janis/Documents/uni_ss19/CG/proof/source/CGTutorial/source/res/models/nanosuit.obj" );
//    Model carModel( "resources/Car/Chevrolet_Camaro_SS_1.3ds" );
//    Model carModel( "resources/Car/Chevrolet_Camaro_SS_5.fbx" );
//    Model carModel( "resources/Car/Chevrolet_Camaro_SS_bkp3.3ds" );
//    Model carModel( "resources/Car/ram3500.3ds" );
    Model carModel( "resources/Car/Chevrolet_Camaro_SS_bkp3.3ds" );
    Model streetCurveA90Model ( "resources/Street/StreetCurveA90.fbx" );

    StreetObject streetObject_1 (streetCurveA90Model);
    StreetObject streetObject_2 (streetCurveA90Model);

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

    char filename[] = "rua.jpg";
    GLint street_texture = TextureFromFile(filename, "resources/Street");
    
    
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
        view = glm::lookAt(glm::vec3(0,2,-5), // Camera is at (0,2,-5), in World Space
                           glm::vec3(0,0,0),  // and looks at the origin
                           glm::vec3(0,1,0)); // Head is up (set to 0,-1,0 to look upside-down


        // Y-Achse nach oben, X-Achse in den Bildschirm, Z-Achse nach rechts
        view = glm::rotate(view, -1.5708f, glm::vec3( 0.0, 1.0, 0.0));
        //
        // Model matrix : an identity matrix (model will be at the origin)
        // assign the result of glm::mat4(1.0f), which is the identity matrix (Einheitsmatrix) to the variable model.
        model = glm::mat4(1.0f);
        
        // glm:: not nesecary because the glm namespace is used. (... but shows where the fct comers from)
        model = glm::rotate(model, angleX, glm::vec3( 1.0, 0.0, 0.0));
        model = glm::rotate(model, angleY, glm::vec3( 0.0, 1.0, 0.0));
        model = glm::rotate(model, angleZ, glm::vec3( 0.0, 0.0, 1.0));

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
         glm::vec3 lightPos = glm::vec3(4,4,-4);
         glUniform3f(glGetUniformLocation(programID, "LightPosition_worldspace"), lightPos.x, lightPos.y, lightPos.z);
        
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

        // Licht am roboterarm
        //glm::vec4 lpw = model * glm::vec4(0.0, 0.3, 0.0, 1.0); // light position world
        //glUniform3f(glGetUniformLocation(programID, "LightPosition_worldspace"), lpw.x / lpw.w, lpw.y / lpw.w, lpw.z / lpw.w); // lpw.w = normalisierungszahl, ist aber bei uns sowieso 1
        
        
        
        // draw car

        model = save;
        model = drawCar(model);
//        model = glm::translate(model, glm::vec3(-1.5, 0.0, 0.0));
//        double scaleFactor = 1.0 / 2.0;
//        model = glm::scale(model, glm::vec3(scaleFactor, scaleFactor, scaleFactor));
//
//
//
//        model = glm::rotate(model, -95.0f, glm::vec3( 1.0, 0.0, 0.0));
        // Bind our monkey_texture in Texture Unit 0
        //glActiveTexture(GL_TEXTURE0); // da multiple texturing m�glich ist notwendig anzugeben welche grad die gewollte ist f�r dieses Texture Unit.
        //glBindTexture(GL_TEXTURE_2D, monkey_texture);

        sendMVP();
        //drawCube();
        carModel.Draw( shader );

        //Straße
        model = save;
        sendMVP();
        streetObject_1.setTexture(street_texture);
        streetObject_1.draw(shader);

        model = save;
        model = glm::translate(model, glm::vec3(5.0, 0.0, 5.0));
        model = glm::rotate(model, -1.5708f, glm::vec3( 0.0, 1.0, 0.0));
        sendMVP();
        streetObject_2.setTexture(street_texture);
        streetObject_2.draw(shader);

        //get time since game started
        GLint64 timer;
        glGetInteger64v(GL_TIMESTAMP, &timer);
        timer = timer/1000000000.0;
        //display time in window
        std::string text = std::to_string(timer);
        glColor3f(1, 1, 1);
        drawText(text.data(), text.length(), 0, 0);
        //printf("Seconds: %s\n", text.data());

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
    
    // single keys
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
                
              
            // TODO: Gleichzeitig w/s + a/d/u/j
                
                // -1 = backwards, 0 = left, 1 = forward, 2 = right, 3 = -1
            case GLFW_KEY_W:
                movement = movement + speed;
                break;
                
            case GLFW_KEY_S:
                movement = movement - speedBackwards;
                break;
                
            case GLFW_KEY_A:
                turn = turn - 0.1f;
                break;
                
            case GLFW_KEY_D:
                turn = turn + 0.1f;
                break;
                
            case GLFW_KEY_U:
                if ( speed <= 3 ) {
                    speed = speed + 0.5f;
                }
                break;
                
            case GLFW_KEY_J:
                if ( speed >= 0.5 ) {
                    speed = speed - 0.5f;
                }
                break;
                
            default:
                break;
        }
    }
    
    // key combinations
    
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
    glm::mat4 save = model;
    model = glm::scale(save, glm::vec3(5.0,0.01,0.01));
    sendMVP();
    drawCube();
    model=save;
    
    save = model;
    model = glm::scale(save, glm::vec3(0.01, 5.0, 0.01));
    sendMVP();
    drawCube();
    model=save;
    
    save = model;
    model = glm::scale(save, glm::vec3(0.01, 0.01, 5.0));
    sendMVP();
    drawCube();
    model=save;
}

void zeichneSeg(float h)
{
    glm::mat4 Save = model;
    model = glm::translate(model, glm::vec3(0.0, h / 2, 0.0)); // wie in schwarzer zeichnung (leserichtung aufw�rts)
    model = glm::scale(model, glm::vec3(h / 6.0, h / 2, h / 6.0 ));
    //Model = glm::translate(Model, glm::vec3(0.0, 1.0, 0.0)); // alternativl�sung, wie in blauer zeichnung (leserichtung aufw�rts)
    
    sendMVP();
    // radius = 1; bezugskoordinatensystem: mittelpunkt des koordinatensystems im mittelpunkt
    drawSphere(10.0, 10.0);
    model=Save;
}

glm::mat4 drawCar(glm::mat4 model)
{
//    zeichneKS();


    model = glm::translate(model, glm::vec3(-1.5, 0.0, 0.0));
    double scaleFactor = 1.0 / 10.0;
    model = glm::scale(model, glm::vec3(scaleFactor, scaleFactor, scaleFactor));
    
    // initial rotation
    model = glm::rotate(model, -95.0f, glm::vec3( 1.0, 0.0, 0.0));

    
    model = glm::translate(model, glm::vec3(0.0, -movement, 0.0));
    model = glm::rotate(model, turn, glm::vec3( 0.0, 0.0, 1.0));
    

    // Problem: nach translate dreht sich das auto bei rotate immmernoch um den ursprünglichen punkt (anstatt um den mittelpunkt des autos)
    // daher müssen wir im world space translaten anstatt im object space, da sonst
    
    // wir brauchebn die einzelnen transformationsmatrizen (rotationsmatrix, translationsmatrix), diese dann mtieinander multiplizieren und auf model anwenden 
    return model;
}

void drawText(const char *text, int length, int x, int y) {

    glMatrixMode(GL_PROJECTION);
    double *matrix = new double[16];
    glGetDoublev(GL_PROJECTION_MATRIX, matrix);
    glLoadIdentity();
    glOrtho(0, 800, 0, 600, -5, 5);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glPushMatrix();
    glLoadIdentity();

    glRasterPos2i(x, y);
    for (int i = 0; i < length; i++) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10 , (int)text[i]);
        //glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, text[i]);
    }

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixd(matrix);
    glMatrixMode(GL_MODELVIEW);
}
