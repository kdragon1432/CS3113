#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION
#define GL_GLEXT_PROTOTYPES 1
#define LOG(argument) std::cout << argument << '\n'

#ifdef _WINDOWS
#include <GL/glew.h>
#endif


#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"                // 4x4 Matrix
#include "glm/gtc/matrix_transform.hpp"  // Matrix transformation methods
#include "ShaderProgram.h"  
#include "stb_image.h"


const int	WINDOW_WIDTH = 640,
WINDOW_HEIGHT = 480;

// Background Color: WHITE
const float		BG_RED = 1.0f,
BG_BLUE = 1.0f,
BG_GREEN = 1.0f,
BG_OPACITY = 1.0f;

// Our viewport—or our "camera"'s—position and dimensions
const int		VIEWPORT_X = 0,
VIEWPORT_Y = 0,
VIEWPORT_WIDTH = WINDOW_WIDTH,
VIEWPORT_HEIGHT = WINDOW_HEIGHT;


// Our shader filepaths; these are necessary for a number of things
// Not least, to actually draw our shapes 
// We'll have a whole lecture on these later
const char		V_SHADER_PATH[] = "shaders/vertex.glsl",
F_SHADER_PATH[] = "shaders/fragment.glsl";

const char VT_SHADER_PATH[] = "shaders/vertex_textured.glsl",
FT_SHADER_PATH[] = "shaders/fragment_textured.glsl";


SDL_Window* displayWindow;
bool gameIsRunning = true;


ShaderProgram BALL;
glm::mat4 view_matrix, model_matrix, projection_matrix;
const char BALL_SPRITE_FILEPATH[] = "ball.png";
GLuint ball_texture_id;

const int NUMBER_OF_TEXTURES = 1; // to be generated, that is
const GLint LEVEL_OF_DETAIL = 0;  // base image level; Level n is the nth mipmap reduction image
const GLint TEXTURE_BORDER = 0;   // this value MUST be zero

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
	displayWindow = SDL_CreateWindow("HW2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
	glewInit();
#endif
	// INITIALIZE OUR CAMERA
	glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
	// LOAD UP OUR SHADERS
	BALL.Load(VT_SHADER_PATH, FT_SHADER_PATH);

	// Initialise our view, model, and projection matrices
	view_matrix = glm::mat4(1.0f);  
	model_matrix = glm::mat4(1.0f); 
	projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

	BALL.SetProjectionMatrix(projection_matrix);
	BALL.SetViewMatrix(view_matrix);
	// Notice we haven't set our model matrix yet!
	//program.SetColor(1.0f, 0.4f, 0.4f, 1.0f);

	// Each object has its own unique ID
	glUseProgram(BALL.programID);


	glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY); // Background: WHITE

	ball_texture_id = load_texture(BALL_SPRITE_FILEPATH);

	// enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		// NO INPUT
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
			gameIsRunning = false;
		}
	}

}


int frame_counter = 0;
float previous_ticks = 0;
//const float ROT_ANGLE = glm::radians(90.0f);
void update() {
	float ticks = (float)SDL_GetTicks() / 1000.0f;  // get the current number of ticks
	float delta_time = ticks - previous_ticks;
	previous_ticks = ticks;
	glm::vec3 scale_vector;
	//frame_counter += 1;

	//model_matrix = glm::rotate(model_matrix, (ROT_ANGLE * delta_time), glm::vec3(0.0f, 0.0f, 1.0f));

}

void render() {
	glClear(GL_COLOR_BUFFER_BIT);

	BALL.SetModelMatrix(model_matrix);

	float vertices[] =
	{
		-0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f,		// triangle 1
		-0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f		// triangle 2
	};

	float texture_coordinates[] = {
		0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,     // triangle 1
		0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,     // triangle 2
	};

	glVertexAttribPointer(BALL.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(BALL.positionAttribute);

	glVertexAttribPointer(BALL.texCoordAttribute, 2, GL_FLOAT, false, 0, texture_coordinates);
	glEnableVertexAttribArray(BALL.texCoordAttribute);

	glBindTexture(GL_TEXTURE_2D, ball_texture_id);
	glDrawArrays(GL_TRIANGLES, 0, 6);


	glDisableVertexAttribArray(BALL.positionAttribute);
	glDisableVertexAttribArray(BALL.texCoordAttribute);

	SDL_GL_SwapWindow(displayWindow);

}

void shutdown() {
	SDL_Quit();
}

int main(int argc, char* argv[]) {
	initialize();

	while (gameIsRunning) {
		process_input();
		update();
		render();
	}

	shutdown();
	return 0;
}

/**
* Author: Kelly Dong
* Assignment: Pong Clone
* Date due: 2023-06-23, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.


#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1
#define STB_IMAGE_IMPLEMENTATION
#define LOG(argument) std::cout << argument << '\n'

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"                // 4x4 Matrix
#include "glm/gtc/matrix_transform.hpp"  // Matrix transformation methods
#include "ShaderProgram.h"  
#include "stb_image.h"


const int	WINDOW_WIDTH  = 640,
			WINDOW_HEIGHT = 480;

// Background Color: WHITE
const float		BG_RED		= 1.0f, // LIGHT BLUE 
				BG_BLUE		= 1.0f, // PINK
				BG_GREEN	= 1.0f, // YELLOW
				BG_OPACITY  = 1.0f;

// Our viewport—or our "camera"'s—position and dimensions
const int		VIEWPORT_X = 0,
				VIEWPORT_Y = 0,
				VIEWPORT_WIDTH  = WINDOW_WIDTH,
				VIEWPORT_HEIGHT = WINDOW_HEIGHT;



// SOLID COLOR SHADERS
const char		V_SHADER_PATH[] = "shaders/vertex.glsl",
				F_SHADER_PATH[] = "shaders/fragment.glsl";
// TEXTURED SHADERS
const char		VT_SHADER_PATH[] = "shaders/vertex_textured.glsl",
				FT_SHADER_PATH[] = "shaders/fragment_textured.glsl";



		


SDL_Window* displayWindow;
bool gameIsRunning = true;


ShaderProgram program;
	glm::mat4	view_matrix,        // Defines the position (location and orientation) of the camera
				model_matrix,       // Defines every translation, rotation, and/or scaling applied to an object; we'll look at these next week
				projection_matrix;  // Defines the characteristics of your camera, such as clip panes, field of view, projection method, etc.
const char BALL_SPRITE[] = "ball.png";
GLuint ball_texture_id;


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
	displayWindow = SDL_CreateWindow("HW2 PONG CLONE", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
	glewInit();
#endif
	// INITIALIZE OUR CAMERA
	glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

	program.Load(VT_SHADER_PATH, FT_SHADER_PATH);

	model_matrix = glm::mat4(1.0f);
	view_matrix = glm::mat4(1.0f);  // Defines the position (location and orientation) of the camera
	projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);  // Defines the characteristics of your camera, such as clip planes, field of view, projection method etc.

	program.SetProjectionMatrix(projection_matrix);
	program.SetViewMatrix(view_matrix);
	// Notice we haven't set our model matrix yet!

	glUseProgram(program.programID);

	glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);

	ball_texture_id = load_texture(BALL_SPRITE);

	// enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void process_input() {
	SDL_Event event;
	while (SDL_PollEvent(&event)){
		switch (event.type){
			case SDL_QUIT:
			case SDL_WINDOWEVENT_CLOSE:
				gameIsRunning = false;
				break;
			default:
				break;
		}
	}
}


int frame_counter = 0;
float previous_ticks = 0;
const float ROT_ANGLE = glm::radians(90.0f);
void update() {
	float ticks = (float)SDL_GetTicks() / 1000.0f;  // get the current number of ticks
	float delta_time = ticks - previous_ticks;
	previous_ticks = ticks;
	glm::vec3 scale_vector;
	//frame_counter += 1;

	model_matrix = glm::rotate(model_matrix, (ROT_ANGLE*delta_time), glm::vec3(0.0f, 0.0f, 1.0f));

}

void render() {
	glClear(GL_COLOR_BUFFER_BIT);

	// Vertices
	float vertices[] = {
		-0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f,  // triangle 1
		-0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f   // triangle 2
	};

	glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(program.positionAttribute);

	// Textures
	float texture_coordinates[] = {
		0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,     // triangle 1
		0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,     // triangle 2
	};

	glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texture_coordinates);
	glEnableVertexAttribArray(program.texCoordAttribute);

	// Bind texture
	glBindTexture(GL_TEXTURE_2D, ball_texture_id);
	glDrawArrays(GL_TRIANGLES, 0, 6); // we are now drawing 2 triangles, so we use 6 instead of 3

	// We disable two attribute arrays now
	glDisableVertexAttribArray(program.positionAttribute);
	glDisableVertexAttribArray(program.texCoordAttribute);

	SDL_GL_SwapWindow(displayWindow);

}

void shutdown() {
	SDL_Quit();
}

int main(int argc, char* argv[]) {
	initialize();

	while (gameIsRunning) {
		process_input();
		update();
		render();
	}

	shutdown();
	return 0;
}
**/