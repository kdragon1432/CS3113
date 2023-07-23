#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION
#define NUMBER_OF_ENEMIES 1
#define LOG(argument) std::cout << argument << '\n'
#define GL_GLEXT_PROTOTYPES 1


#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"
#include "cmath"
#include <ctime>
#include <vector>
#include "Entity.h" 

const int WINDOW_HEIGHT = 480, WINDOW_WIDTH = 640;
const float BG_RED=1.0f, BG_GREEN=1.0f, BG_BLUE=1.0f, BG_OPACITY=1.0f;

const int	VIEWPORT_X		= 0,
			VIEWPORT_Y		= 0,
			VIEWPORT_WIDTH	= WINDOW_WIDTH,
			VIEWPORT_HEIGHT	= WINDOW_HEIGHT;

const char	V_SHADER_PATH[] = "shaders/vertex.glsl",
			F_SHADER_PATH[] = "shaders/fragment.glsl";

const char	V_TEXTURE_PATH[] = "shaders/vertex_textured.glsl",
			F_TEXTURE_PATH[] = "shaders/fragment_textured.glsl";

ShaderProgram program;
glm::mat4 view_matrix, model_matrix, projection_matrix;


const float MILLISECONDS_IN_SECOND = 1000.0f;
float previous_ticks = 0.0f; //this changes -> no const

SDL_Window* displayWindow;
bool gameIsRunning = true;


struct GameState {
	Entity* enemies[NUMBER_OF_ENEMIES];
};

GameState state;


// LOAD_TEXTURE CONSTANTS
const int NUMBER_OF_TEXTURES = 1;  // to be generated, that is
const GLint LEVEL_OF_DETAIL = 0;  // base image level; Level n is the nth mipmap reduction image
const GLint TEXTURE_BORDER = 0;  // this value MUST be zero

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

	// STEP 3: Setting our texture filter modes
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// STEP 4: Setting our texture wrapping modes
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // the last argument can change depending on what you are looking for
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// STEP 5: Releasing our file from memory and returning our texture id
	stbi_image_free(image);

	return textureID;
}


void initialise() {
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("HW4! RISE OF THE AI!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
	glewInit();
#endif
	glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
	program.Load(V_SHADER_PATH, F_SHADER_PATH);
	program.Load(V_TEXTURE_PATH, F_TEXTURE_PATH);
	view_matrix = glm::mat4(1.0f);
	model_matrix = glm::mat4(1.0f);
	projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

	program.SetProjectionMatrix(projection_matrix);
	program.SetViewMatrix(view_matrix);

	program.SetColor(0.0f, 0.4f, 0.4f, 1.0f);
	glUseProgram(program.programID);

	glClearColor(BG_RED, BG_GREEN, BG_BLUE, BG_OPACITY);
}




void process_input() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
		case SDL_WINDOWEVENT_CLOSE:
			gameIsRunning = false;
			break;
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
			case SDLK_q:
				gameIsRunning = false;
				break;
			}
			break;
		}
	}
}



void update() {
	float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND; // get the current number of ticks
	float delta_time = ticks - previous_ticks; // the delta time is the difference from the last frame
	previous_ticks = ticks;

}

void render() {
	glClear(GL_COLOR_BUFFER_BIT);
	program.SetModelMatrix(model_matrix);

	// Step 3
	float vertices[] =
	{
		 0.5f, -0.5f,  // (x1, y1)
		 0.0f,  0.5f,  // (x2, y2)
		-0.5f, -0.5f   // (x3, y3)
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
	initialise();
	
	
	while (gameIsRunning) {
		process_input();
		update();
		render();	
	}

	shutdown();
	return 0;
}