/**
* Author: Kelly Dong
* Assignment: Simple 2D Scene
* Date due: 2023-06-11, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/
#include <iostream>

#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1
#define LOG(argument) std::cout << argument << '\n'
#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

const int   WINDOW_WIDTH = 640,
            WINDOW_HEIGHT = 480;

const float BG_RED = 0.1922f,
            BG_BLUE = 0.549f,
            BG_GREEN = 0.9059f,
            BG_OPACITY = 1.0f;

const int   VIEWPORT_X = 0,
            VIEWPORT_Y = 0,
            VIEWPORT_WIDTH = WINDOW_WIDTH,
            VIEWPORT_HEIGHT = WINDOW_HEIGHT;

const char  V_SHADER_PATH[] = "shaders/vertex.glsl",
            F_SHADER_PATH[] = "shaders/fragment.glsl";

const char  V_SHADER_PATH_T[] = "shaders/vertex_textured.glsl",
            F_SHADER_PATH_T[] = "shaders/fragment_textured.glsl";

const int   TRIANGLE_RED = 1.0,
            TRIANGLE_BLUE = 0.4,
            TRIANGLE_GREEN = 0.4,
            TRIANGLE_OPACITY = 1.0;

const float GROWTH_FACTOR = 2.00f;
const float SHRINK_FACTOR = 0.50f;
const int MAX_FRAME = 4000;

int frame_counter = 0;
bool is_growing = true;
bool rotate = true;
const float ROT_ANGLE = glm::radians(0.05f);
float TRAN_VALUE = -0.025f;

ShaderProgram program1;
glm::mat4 view_matrix1, model_matrix1, projection_matrix1, trans_matrix1;
const char PLAYER1_SPRITE[] = "player1.png";
GLuint player1_texture_id;

ShaderProgram program2;
glm::mat4 view_matrix2, model_matrix2, projection_matrix2, trans_matrix2;
const char PLAYER2_SPRITE[] = "player2.png";
GLuint player2_texture_id;

SDL_Window* displayWindow;
bool gameIsRunning = true;



const int NUMBER_OF_TEXTURES = 1; // to be generated, that is
const GLint LEVEL_OF_DETAIL = 0; // base image level; Level n is the nth mipmap reduction image
const GLint TEXTURE_BORDER = 0; // this value MUST be zero

GLuint load_texture(const char* filepath)
{
    // STEP 1: Loading the image file
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);

    if (image == NULL)
    {
        LOG("Unable to load image. Make sure the path is correct.");
        assert(false);
    }

    // STEP 2: Generating and binding a texture ID to our image
    GLuint textureID;
    glGenTextures(NUMBER_OF_TEXTURES, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);

    // STEP 3: Setting our texture filter parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // STEP 4: Releasing our file from memory and returning our texture id
    stbi_image_free(image);

    return textureID;
}



void initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("HW1!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    program1.Load(V_SHADER_PATH_T, F_SHADER_PATH_T);
    program2.Load(V_SHADER_PATH_T, F_SHADER_PATH_T);

    //PR0GRAM1 INIT
    view_matrix1 = glm::mat4(1.0f);      // Defines the position (location and orientation) of the camera
    model_matrix1 = glm::mat4(1.0f);     // Defines every translation, rotations, or scaling applied to an object
    projection_matrix1 = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);  // Defines the characteristics of your camera, such as clip planes, field of view, projection method etc.
    trans_matrix1 = model_matrix1;

    //PROGRAM2 INIT
    view_matrix2 = glm::mat4(1.0f); 
    model_matrix2 = glm::mat4(1.0f); 
    projection_matrix2 = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    trans_matrix2 = model_matrix2;

    program1.SetProjectionMatrix(projection_matrix1);
    program1.SetViewMatrix(view_matrix1);

    program2.SetProjectionMatrix(projection_matrix2);
    program2.SetViewMatrix(view_matrix2);
    // Notice we haven't set our model matrix yet!

    //program1.SetColor(TRIANGLE_RED, TRIANGLE_BLUE, TRIANGLE_GREEN, TRIANGLE_OPACITY);
    //program2.SetColor(0, 0, 1, TRIANGLE_OPACITY);
    player1_texture_id = load_texture(PLAYER1_SPRITE);
    player2_texture_id = load_texture(PLAYER2_SPRITE);

    glUseProgram(program1.programID);
    glUseProgram(program2.programID);
    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 

}

void processing_input() {
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        // NO INPUT
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE)
        {gameIsRunning = false;}
    }
}

void update()
{
    frame_counter += 1;

    if (frame_counter == 2000) {
        TRAN_VALUE *= -1;
    }
    if (frame_counter == 4000) {
        frame_counter = 0;
        is_growing = !is_growing;
        rotate = !rotate;
        if (is_growing == true) {
            model_matrix2 = glm::scale(model_matrix2, glm::vec3(GROWTH_FACTOR, GROWTH_FACTOR, 0.0F));
        } else { model_matrix2 = glm::scale(model_matrix2, glm::vec3(SHRINK_FACTOR, SHRINK_FACTOR, 0.0F)); }
        TRAN_VALUE *= -1;
    }

    if (rotate == true) {
        model_matrix1 = glm::rotate(model_matrix1, ROT_ANGLE, glm::vec3(0.0f, 0.0f, 0.5f));
    }
    else {
        model_matrix1 = glm::translate(model_matrix1, glm::vec3(TRAN_VALUE/10, -TRAN_VALUE/10, 0.0F));
    }
    model_matrix2 = glm::translate(model_matrix2, glm::vec3(TRAN_VALUE/10, TRAN_VALUE/10, 0.0f));

}

void render() {
    glClear(GL_COLOR_BUFFER_BIT);

    program1.SetModelMatrix(model_matrix1);
    

    float vertices1[] =
    {
        -0.5f, -0.5f,
        0.0f, 0.5f,
        0.5f, -0.5f
    };

    glVertexAttribPointer(program1.positionAttribute, 2, GL_FLOAT, false, 0, vertices1);
    glEnableVertexAttribArray(program1.positionAttribute);

    float textures1[] =
    {
        0.0, 1.0,
        0.5, 0.0,
        1.0, 1.0

    };

    glVertexAttribPointer(program1.texCoordAttribute, 2, GL_FLOAT, false, 0, textures1);
    glEnableVertexAttribArray(program1.texCoordAttribute);

    glBindTexture(GL_TEXTURE_2D, player1_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glDisableVertexAttribArray(program1.positionAttribute);
    glDisableVertexAttribArray(program1.texCoordAttribute);


    program2.SetModelMatrix(model_matrix2);
    float vertices2[] =
    {
        2.0f, 2.0f,
        2.5f, 3.0f,
        3.0f, 2.0f
    };
    glVertexAttribPointer(program2.positionAttribute, 2, GL_FLOAT, false, 0, vertices2);
    glEnableVertexAttribArray(program2.positionAttribute);

    float textures2[] =
    {
        0.0, 1.0,
        0.5, 0.0,
        1.0, 1.0
    };
    glVertexAttribPointer(program2.texCoordAttribute, 2, GL_FLOAT, false, 0, textures2);
    glEnableVertexAttribArray(program2.texCoordAttribute);

    glBindTexture(GL_TEXTURE_2D, player2_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glDisableVertexAttribArray(program2.positionAttribute);
    glDisableVertexAttribArray(program2.texCoordAttribute);

    SDL_GL_SwapWindow(displayWindow);
}



void shutdown() { SDL_Quit(); }

int main(int argc, char* argv[]) {
    initialize();
	while (gameIsRunning){
        processing_input();
        update();
        render();
	}

    shutdown();
	return 0;
}








