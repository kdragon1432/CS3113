/**
* Author: Kelly Dong
* Assignment: Pong Clone
* Date due: 2023-06-23, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"                // 4x4 Matrix
#include "glm/gtc/matrix_transform.hpp"  // Matrix transformation methods
#include "ShaderProgram.h"  


const int	WINDOW_WIDTH  = 640,
			WINDOW_HEIGHT = 480;

// Background Color: WHITE
const float		BG_RED		= 1.0f, 
				BG_BLUE		= 1.0f, 
				BG_GREEN	= 1.0f, 
				BG_OPACITY  = 1.0f;

// Our viewport—or our "camera"'s—position and dimensions
const int		VIEWPORT_X = 0,
				VIEWPORT_Y = 0,
				VIEWPORT_WIDTH  = WINDOW_WIDTH,
				VIEWPORT_HEIGHT = WINDOW_HEIGHT;


// Our shader filepaths; these are necessary for a number of things
// Not least, to actually draw our shapes 
// We'll have a whole lecture on these later
const char		V_SHADER_PATH[] = "shaders/vertex.glsl",
				F_SHADER_PATH[] = "shaders/fragment.glsl";
		


SDL_Window* displayWindow;
bool gameIsRunning = true;


ShaderProgram program;
glm::mat4 view_matrix,        // Defines the position (location and orientation) of the camera
model_matrix,       // Defines every translation, rotation, and/or scaling applied to an object; we'll look at these next week
projection_matrix;  // Defines the characteristics of your camera, such as clip panes, field of view, projection method, etc.


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
	program.Load(V_SHADER_PATH, F_SHADER_PATH);

	// Initialise our view, model, and projection matrices
	view_matrix = glm::mat4(1.0f);  // Defines the position (location and orientation) of the camera
	model_matrix = glm::mat4(1.0f);  // Defines every translation, rotations, or scaling applied to an object
	projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);  // Defines the characteristics of your camera, such as clip planes, field of view, projection method etc.

	program.SetProjectionMatrix(projection_matrix);
	program.SetViewMatrix(view_matrix);
	// Notice we haven't set our model matrix yet!
	program.SetColor(1.0f, 0.4f, 0.4f, 1.0f);

	// Each object has its own unique ID
	glUseProgram(program.programID);


	glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY); // Background: WHITE

}

void process_input() {
	SDL_Event event;
	while (SDL_PollEvent(&event)){
		// NO INPUT
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
			gameIsRunning = false;
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

	program.SetModelMatrix(model_matrix);

	float vertices[] =
	{
		 0.5f, -0.5f, // (x1,y1)
		 0.0f,  0.5f, // (x2,y2)
		-0.5f, -0.5f // (x3,y3)
	};

	glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(program.positionAttribute);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glDisableVertexAttribArray(program.positionAttribute);

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