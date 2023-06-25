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
glm::mat4 ball_view, ball_matrix, ball_projection;
const char BALL_SPRITE_FILEPATH[] = "ball.png";
GLuint ball_texture_id;

glm::vec3 ball_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 ball_movement = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3 ball_orientation = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 ball_rotation = glm::vec3(0.0f, 0.0f, 0.0f);

ShaderProgram p1;
glm::mat4 p1_view, p1_matrix, p1_projection;
const char PLAYER1_SPRITE_FILEPATH[] = "player1.png";
GLuint p1_texture_id;

glm::vec3 p1_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 p1_movement = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3 p1_orientation = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 p1_rotation = glm::vec3(0.0f, 0.0f, 0.0f);

ShaderProgram p2;
glm::mat4 p2_view, p2_matrix, p2_projection;
const char PLAYER2_SPRITE_FILEPATH[] = "player2.png";
GLuint p2_texture_id;

glm::vec3 p2_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 p2_movement = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3 p2_orientation = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 p2_rotation = glm::vec3(0.0f, 0.0f, 0.0f);






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
	p1.Load(VT_SHADER_PATH, FT_SHADER_PATH);
	p2.Load(VT_SHADER_PATH, FT_SHADER_PATH);

	// Initialise our view, model, and projection matrices
	ball_view = glm::mat4(1.0f);  
	ball_matrix = glm::mat4(1.0f); 
	ball_projection = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
	p1_view = glm::mat4(1.0f);
	p1_matrix = glm::mat4(1.0f);
	p1_projection = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
	p2_view = glm::mat4(1.0f);
	p2_matrix = glm::mat4(1.0f);
	p2_projection = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

	BALL.SetProjectionMatrix(ball_projection);
	BALL.SetViewMatrix(ball_view);
	p1.SetProjectionMatrix(p1_projection);
	p1.SetViewMatrix(p1_view); 
	p2.SetProjectionMatrix(p2_projection);
	p2.SetViewMatrix(p2_view);


	//p1.SetColor(1.0f, 0.4f, 0.4f, 1.0f);

	// Each object has its own unique ID
	glUseProgram(BALL.programID);
	glUseProgram(p1.programID);
	glUseProgram(p2.programID);


	glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY); // Background: WHITE

	ball_texture_id = load_texture(BALL_SPRITE_FILEPATH);
	p1_texture_id = load_texture(PLAYER1_SPRITE_FILEPATH);
	p2_texture_id = load_texture(PLAYER2_SPRITE_FILEPATH);

	// enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input(){
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_QUIT:
			case SDL_WINDOWEVENT_CLOSE:
				gameIsRunning = false;
				break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
				case SDLK_UP:
					p2_movement.y = 1.0f;
					break;
				case SDLK_DOWN:
					p2_movement.y = -1.0f;
					break;
				case SDLK_w:
					p1_movement.y = 1.0f;
					break;
				case SDLK_s:
					p1_movement.y = -1.0f;
					break;
				default:
					break;
				}
			case SDL_KEYUP:
				switch (event.key.keysym.sym) {
				case SDLK_UP:
					p2_movement.y = 0.0f;
					break;
				case SDLK_DOWN:
					p2_movement.y = 0.0f;
					break;
				case SDLK_w:
					p1_movement.y = 0.0f;
					break;
				case SDLK_s:
					p1_movement.y = 0.0f;
					break;
				default:
					break;
				}
			default:
				break;
		}
	}
	const Uint8* key_state = SDL_GetKeyboardState(NULL);

	if (key_state[SDL_SCANCODE_UP]) {
		p2_movement.y = 1.0f;
	}
	else if (key_state[SDL_SCANCODE_DOWN]) {
		p2_movement.y = -1.0f;
	}
	if (key_state[SDL_SCANCODE_W])
	{
		p1_movement.y = 1.0f;
	}
	else if (key_state[SDL_SCANCODE_S]){
		p1_movement.y = -1.0f;
	}

	// This makes sure that the player can't "cheat" their way into moving faster
	if (glm::length(p2_movement) > 1.0f){
		p2_movement = glm::normalize(p2_movement);
	}
	if (glm::length(p1_movement) > 1.0f) {
		p1_movement = glm::normalize(p1_movement);
	}
}


int frame_counter = 0;
float previous_ticks = 0;
float player_speed = 5.0f;  // move 1 unit per second
//const float ROT_ANGLE = glm::radians(90.0f);
void update() {
	float ticks = (float)SDL_GetTicks() / 1000.0f;  // get the current number of ticks
	float delta_time = ticks - previous_ticks;
	previous_ticks = ticks;
	

	// Add direction * units per second * elapsed time
	p2_position += p2_movement * player_speed * delta_time;
	p1_position += p1_movement * player_speed * delta_time;

	p2_matrix = glm::mat4(1.0f);
	p2_matrix = glm::translate(p2_matrix, p2_position);
	p1_matrix = glm::mat4(1.0f);
	p1_matrix = glm::translate(p1_matrix, p1_position);

	//model_matrix = glm::rotate(model_matrix, (ROT_ANGLE * delta_time), glm::vec3(0.0f, 0.0f, 1.0f));

}

void render() {
	glClear(GL_COLOR_BUFFER_BIT);

	BALL.SetModelMatrix(ball_matrix);

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

	p1.SetModelMatrix(p1_matrix);

	float vertices1[] =
	{
		-4.9f, 0.0f, 
		-4.5f, -1.0f, 
		-4.5f, 1.0f,
	};

	float texture_coordinates1[] = {
		0.0f, 0.5f, 
		0.4f, 0.0f, 
		0.4f, 0.1f, 

	};

	glVertexAttribPointer(p1.positionAttribute, 2, GL_FLOAT, false, 0, vertices1);
	glEnableVertexAttribArray(p1.positionAttribute);
	glVertexAttribPointer(p1.texCoordAttribute, 2, GL_FLOAT, false, 0, texture_coordinates1);
	glEnableVertexAttribArray(p1.texCoordAttribute);
	glBindTexture(GL_TEXTURE_2D, p1_texture_id);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glDisableVertexAttribArray(p1.positionAttribute);
	glDisableVertexAttribArray(p1.texCoordAttribute);

	p2.SetModelMatrix(p2_matrix);

	float vertices2[] =
	{
		4.9f, 0.0f,
		4.5f, -1.0f,
		4.5f, 1.0f,
	};

	float texture_coordinates2[] = {
		1.0f, 0.5f,
		0.6f, 0.0f,
		0.6f, 0.1f,

	};

	glVertexAttribPointer(p2.positionAttribute, 2, GL_FLOAT, false, 0, vertices2);
	glEnableVertexAttribArray(p2.positionAttribute);
	glVertexAttribPointer(p2.texCoordAttribute, 2, GL_FLOAT, false, 0, texture_coordinates2);
	glEnableVertexAttribArray(p2.texCoordAttribute);
	glBindTexture(GL_TEXTURE_2D, p2_texture_id);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glDisableVertexAttribArray(p2.positionAttribute);
	glDisableVertexAttribArray(p2.texCoordAttribute);

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