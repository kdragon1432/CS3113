/**
* Author: Kelly Dong
* Assignment: Lunar Lander
* Date due: 2023-07-07, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION
#define NUMBER_OF_ENEMIES 3
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

const int WINDOW_WIDTH = 640,
WINDOW_HEIGHT = 480;

const float BG_RED = 0.0f,
BG_BLUE = 0.0f,
BG_GREEN = 0.0f,
BG_OPACITY = 1.0f;

const int VIEWPORT_X = 0,
VIEWPORT_Y = 0,
VIEWPORT_WIDTH = WINDOW_WIDTH,
VIEWPORT_HEIGHT = WINDOW_HEIGHT;

const char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram g_program;
glm::mat4 g_view_matrix, g_projection_matrix;

struct GameState
{
	Entity* player;
	Entity* target;
	Entity* stop[3];

};

float max_fuel = 100;
float fuel = 100;

GameState g_state;


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
	displayWindow = SDL_CreateWindow("HW3! LUNAR LANDER", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
	glewInit();
#endif
	glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

	g_program.Load(V_SHADER_PATH, F_SHADER_PATH);

	g_view_matrix = glm::mat4(1.0f);  // Defines the position (location and orientation) of the camera
	g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);  // Defines the characteristics of your camera, such as clip planes, field of view, projection method etc.

	g_program.SetProjectionMatrix(g_projection_matrix);
	g_program.SetViewMatrix(g_view_matrix);

	glUseProgram(g_program.programID);

	GLuint player_texture_id = load_texture("assets/ufo.png");
	g_state.player = new Entity();
	g_state.player->m_speed = 1.0f;
	g_state.player->m_texture_id = player_texture_id;
	g_state.player->isWinner = 0;
	g_state.player->set_position(glm::vec3(-3.0f, 3.5f, 0.0f)); // UFO STARTING POSITION
	g_state.player->m_movement = glm::vec3(0);
	g_state.player->m_acceleration = glm::vec3(0.0f, -0.5f, 0.0f);

	GLuint target_texture_id = load_texture("assets/target.png");
	g_state.target = new Entity();
	g_state.target->m_speed = 0.0f;
	g_state.target->m_texture_id = target_texture_id;
	g_state.target->set_position(glm::vec3(2.5f, -3.25f, 0.0f));
	g_state.target->m_movement = glm::vec3(0);

	GLuint stop_texture_id = load_texture("assets/stop.png");
	for (int i = 0; i < 3; i++) {
		g_state.stop[i] = new Entity();
		g_state.stop[i]->m_texture_id = stop_texture_id;
	}
	g_state.stop[0]->set_position(glm::vec3(-3.5f, 0.5f, 0.0f));
	g_state.stop[1]->set_position(glm::vec3(1.5f, 0.5f, 0.0f));
	g_state.stop[2]->set_position(glm::vec3(4.0f, -2.0f, 0.0f));

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
					default:
						break;
				}
			default:
				break;
		}
	}
	const Uint8* key_state = SDL_GetKeyboardState(NULL);

	if (key_state[SDL_SCANCODE_LEFT])
	{
		fuel -= 1;
		if (fuel > 0) {
			g_state.player->m_acceleration.x += -0.1f;
		}
	}
	else if (key_state[SDL_SCANCODE_RIGHT])
	{
		fuel -= 1;
		if (fuel > 0) {
			g_state.player->m_acceleration.x += 0.1f;
		}
	}
	else {
		g_state.player->m_acceleration.x = 0.0f;
		if (g_state.player->get_movement().x < 0.0f) {
			g_state.player->m_acceleration.x += 0.1f;
		}
		if (g_state.player->get_movement().x > 0.0f) {
			g_state.player->m_acceleration.x += -0.1f;
		}
	}
	if (glm::length(g_state.player->m_movement) > 1.0f)
	{
		g_state.player->m_movement = glm::normalize(g_state.player->m_movement);
	}

	float dist_x = pow((g_state.target->get_position().x - g_state.player->get_position().x), 2);
	float dist_y = pow((g_state.target->get_position().y - g_state.player->get_position().y), 2);

	if (sqrt(dist_x+dist_y) < 0.3f) {
		g_state.player->m_movement = glm::vec3(0);
		g_state.player->m_acceleration = glm::vec3(0);
		g_state.player->isWinner = 1;
	}

	if (g_state.player->CheckCollision(g_state.stop[0])) {
		g_state.player->m_movement = glm::vec3(0);
		g_state.player->m_acceleration = glm::vec3(0);
		g_state.player->isWinner = 2;
	}
	if (g_state.player->CheckCollision(g_state.stop[1])) {
		g_state.player->m_movement = glm::vec3(0);
		g_state.player->m_acceleration = glm::vec3(0);
		g_state.player->isWinner = 2;
	}
	if (g_state.player->CheckCollision(g_state.stop[2])) {
		g_state.player->m_movement = glm::vec3(0);
		g_state.player->m_acceleration = glm::vec3(0);
		g_state.player->isWinner = 2;
	}

	if (g_state.player->get_position().x < -5.0f || g_state.player->get_position().x > 5.0f) {
		g_state.player->m_movement = glm::vec3(0);
		g_state.player->m_acceleration = glm::vec3(0);
		g_state.player->isWinner = 2;

	}
	if (g_state.player->get_position().y < -4.0f || g_state.player->get_position().y > 4.0f) {
		g_state.player->m_movement = glm::vec3(0);
		g_state.player->m_acceleration = glm::vec3(0);
		g_state.player->isWinner = 2;
	}

}

float g_previous_ticks = 0.0f;

void update() {
	float ticks = (float)SDL_GetTicks() / 1000.0f; // get the current number of ticks
	float delta_time = ticks - g_previous_ticks; // the delta time is the difference from the last frame
	g_previous_ticks = ticks;

	// Add direction * units per second * elapsed time
	g_state.player->update(delta_time);
	g_state.target->update(delta_time);
	for (int i = 0; i < 3; i++) {
		g_state.stop[i]->update(delta_time);
	}

}

const int FONTBANK_SIZE = 16;

void DrawText(ShaderProgram* program, GLuint font_texture_id, std::string text, float screen_size, float spacing, glm::vec3 position)
{
	// Scale the size of the fontbank in the UV-plane
	// We will use this for spacing and positioning
	float width = 1.0f / FONTBANK_SIZE;
	float height = 1.0f / FONTBANK_SIZE;

	// Instead of having a single pair of arrays, we'll have a series of pairs—one for each character
	// Don't forget to include <vector>!
	std::vector<float> vertices;
	std::vector<float> texture_coordinates;

	// For every character...
	for (int i = 0; i < text.size(); i++) {
		// 1. Get their index in the spritesheet, as well as their offset (i.e. their position
		//    relative to the whole sentence)
		int spritesheet_index = (int)text[i];  // ascii value of character
		float offset = (screen_size + spacing) * i;

		// 2. Using the spritesheet index, we can calculate our U- and V-coordinates
		float u_coordinate = (float)(spritesheet_index % FONTBANK_SIZE) / FONTBANK_SIZE;
		float v_coordinate = (float)(spritesheet_index / FONTBANK_SIZE) / FONTBANK_SIZE;

		// 3. Inset the current pair in both vectors
		vertices.insert(vertices.end(), {
			offset + (-0.5f * screen_size), 0.5f * screen_size,
			offset + (-0.5f * screen_size), -0.5f * screen_size,
			offset + (0.5f * screen_size), 0.5f * screen_size,
			offset + (0.5f * screen_size), -0.5f * screen_size,
			offset + (0.5f * screen_size), 0.5f * screen_size,
			offset + (-0.5f * screen_size), -0.5f * screen_size,
			});

		texture_coordinates.insert(texture_coordinates.end(), {
			u_coordinate, v_coordinate,
			u_coordinate, v_coordinate + height,
			u_coordinate + width, v_coordinate,
			u_coordinate + width, v_coordinate + height,
			u_coordinate + width, v_coordinate,
			u_coordinate, v_coordinate + height,
			});
	}

	// 4. And render all of them using the pairs
	glm::mat4 model_matrix = glm::mat4(1.0f);
	model_matrix = glm::translate(model_matrix, position);

	program->SetModelMatrix(model_matrix);
	glUseProgram(program->programID);

	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices.data());
	glEnableVertexAttribArray(program->positionAttribute);
	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texture_coordinates.data());
	glEnableVertexAttribArray(program->texCoordAttribute);

	glBindTexture(GL_TEXTURE_2D, font_texture_id);
	glDrawArrays(GL_TRIANGLES, 0, (int)(text.size() * 6));

	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
}


void render() {
	glClear(GL_COLOR_BUFFER_BIT);
	
	g_state.target->render(&g_program);
	g_state.player->render(&g_program);

	for (int i = 0; i < 3; i++) {
		g_state.stop[i]->render(&g_program);
	}
	
	if (fuel >= (max_fuel*.90) ){
		DrawText(&g_program, load_texture("assets/font1.png"), "FUEL: 100%", 0.25f, 0.0f, glm::vec3(2.5f, 3.0f, 0.0f));
	}
	if (fuel < (max_fuel * .90)) {
			DrawText(&g_program, load_texture("assets/font1.png"), "FUEL: 90%", 0.25f, 0.0f, glm::vec3(2.5f, 3.0f, 0.0f));
		}
	if (fuel <= (max_fuel * .75)) {
			DrawText(&g_program, load_texture("assets/font1.png"), "FUEL: 75%", 0.25f, 0.0f, glm::vec3(2.5f, 3.0f, 0.0f));
		}
	if (fuel <= (max_fuel * .50)) {
			DrawText(&g_program, load_texture("assets/font1.png"), "FUEL: 50%", 0.25f, 0.0f, glm::vec3(2.5f, 3.0f, 0.0f));
		}
	if (fuel <= (max_fuel * .25)) {
			DrawText(&g_program, load_texture("assets/font1.png"), "FUEL: 25%", 0.25f, 0.0f, glm::vec3(2.5f, 3.0f, 0.0f));
		}
	if (fuel <= (max_fuel * .15)) {
		DrawText(&g_program, load_texture("assets/font1.png"), "FUEL: 10%", 0.25f, 0.0f, glm::vec3(2.5f, 3.0f, 0.0f));
	}
	if (fuel <= 0) {
		DrawText(&g_program, load_texture("assets/font1.png"), "FUEL: 0%", 0.25f, 0.0f, glm::vec3(2.5f, 3.0f, 0.0f));
	}
	


	if (g_state.player->isWinner == 1) {
		DrawText(&g_program, load_texture("assets/font1.png"), "MISSION ACCOMPLISHED!", 0.25f, 0.0f, glm::vec3(-2.0f, 0.0f, 0.0f));
	}
	if (g_state.player->isWinner == 2) {
		DrawText(&g_program, load_texture("assets/font1.png"), "MISSION FAILED!", 0.25f, 0.0f, glm::vec3(-2.0f, 0.0f, 0.0f));
	}

	SDL_GL_SwapWindow(displayWindow);
}

void shutdown() { SDL_Quit(); }



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