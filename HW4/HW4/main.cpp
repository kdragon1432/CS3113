#include <iostream>
#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION
#define LOG(argument) std::cout << argument << '\n'
#define GL_GLEXT_PROTOTYPES 1
#define FIXED_TIMESTEP 0.0166666f
#define ENEMY_COUNT 3
#define LEVEL1_WIDTH 14
#define LEVEL1_HEIGHT 5

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL_mixer.h>
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
#include "Map.h"

// ————— GAME STATE ————— //
struct GameState
{
    Entity* player;
    Entity* enemies;

    Map* map;

    Mix_Music* bgm;
    Mix_Chunk* jump_sfx;
};

int defeated = 0;

// ————— CONSTANTS ————— //
const int WINDOW_WIDTH = 640,
WINDOW_HEIGHT = 480;

const float BG_RED = 0.1922f,
BG_BLUE = 0.549f,
BG_GREEN = 0.9059f,
BG_OPACITY = 1.0f;

const int VIEWPORT_X = 0,
VIEWPORT_Y = 0,
VIEWPORT_WIDTH = WINDOW_WIDTH,
VIEWPORT_HEIGHT = WINDOW_HEIGHT;

const char GAME_WINDOW_NAME[] = "HW4!";

const char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

const float MILLISECONDS_IN_SECOND = 1000.0;

const char SPRITESHEET_FILEPATH[] = "assets/character_005.png",
ENEMY_FILEPATH[] = "assets/shipYellow.png",
MAP_TILESET_FILEPATH[] = "assets/tilemap.png",
BGM_FILEPATH[] = "assets/space_jazz.mp3",
JUMP_SFX_FILEPATH[] = "assets/jump.wav";

const int NUMBER_OF_TEXTURES = 1;
const GLint LEVEL_OF_DETAIL = 0;
const GLint TEXTURE_BORDER = 0;

unsigned int LEVEL_1_DATA[] =
{
    0, 0, 0, 0, 0, 1, 2, 2, 2, 3, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    17,18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 19,
    33,34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 35
};

// ————— VARIABLES ————— //
GameState g_state;

//int isWinner = 0;

SDL_Window* m_display_window;
bool m_game_is_running = true;

ShaderProgram m_program;
glm::mat4 m_view_matrix, m_projection_matrix;

float m_previous_ticks = 0.0f,
m_accumulator = 0.0f;

// ————— GENERAL FUNCTIONS ————— //
GLuint load_texture(const char* filepath)
{
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);

    if (image == NULL)
    {
        LOG("Unable to load image. Make sure the path is correct.");
        assert(false);
    }

    GLuint texture_id;
    glGenTextures(NUMBER_OF_TEXTURES, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    stbi_image_free(image);

    return texture_id;
}

void initialise()
{
    // ————— GENERAL ————— //
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    m_display_window = SDL_CreateWindow(GAME_WINDOW_NAME,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(m_display_window);
    SDL_GL_MakeCurrent(m_display_window, context);

#ifdef _WINDOWS
    glewInit();
#endif

    // ————— VIDEO SETUP ————— //
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    m_program.Load(V_SHADER_PATH, F_SHADER_PATH);

    m_view_matrix = glm::mat4(1.0f);
    m_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    m_program.SetProjectionMatrix(m_projection_matrix);
    m_program.SetViewMatrix(m_view_matrix);

    glUseProgram(m_program.programID);

    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);

    // ————— MAP SET-UP ————— //
    GLuint map_texture_id = load_texture(MAP_TILESET_FILEPATH);
    g_state.map = new Map(LEVEL1_WIDTH, LEVEL1_HEIGHT, LEVEL_1_DATA, map_texture_id, 1.0f, 16, 7);

    // ————— GEORGE SET-UP ————— //
    // Existing
    g_state.player = new Entity();
    g_state.player->isPlayer = true;
    g_state.player->isWinner = 0;
    g_state.player->set_entity_type(PLAYER);
    g_state.player->set_position(glm::vec3(0.0f, 0.0f, 0.0f));
    g_state.player->set_movement(glm::vec3(0.0f));
    g_state.player->set_speed(2.5f);
    g_state.player->set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));
    g_state.player->m_texture_id = load_texture(SPRITESHEET_FILEPATH);

    // Walking
    g_state.player->m_walking[g_state.player->LEFT] = new int[4] { 4, 5, 6, 7 };
    g_state.player->m_walking[g_state.player->RIGHT] = new int[4] { 8, 9, 10, 11 };
    g_state.player->m_walking[g_state.player->UP] = new int[4] { 12, 13, 14, 15 };
    g_state.player->m_walking[g_state.player->DOWN] = new int[4] { 0, 1, 2, 3 };

    g_state.player->m_animation_indices = g_state.player->m_walking[g_state.player->RIGHT];  // start George looking left
    g_state.player->m_animation_frames = 4;
    g_state.player->m_animation_index = 0;
    g_state.player->m_animation_time = 0.0f;
    g_state.player->m_animation_cols = 4;
    g_state.player->m_animation_rows = 4;
    g_state.player->set_height(0.8f);
    g_state.player->set_width(0.8f);

    // Jumping
    g_state.player->m_jumping_power = 8.0f;

    GLuint enemy_texture_id = load_texture(ENEMY_FILEPATH);


    // ---- ENEMY 1---
    g_state.enemies = new Entity[ENEMY_COUNT];
    g_state.enemies[0].set_entity_type(ENEMIES);
    g_state.enemies[0].m_texture_id = enemy_texture_id;
    g_state.enemies[0].set_ai_type(GUARD);
    g_state.enemies[0].set_ai_state(WALKING);
    g_state.enemies[0].set_position(glm::vec3(3.0f, 0.0f, 0.0f));
    g_state.enemies[0].set_movement(glm::vec3(0.0f));
    g_state.enemies[0].set_speed(1.0f);
    g_state.enemies[0].set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));
    g_state.enemies[0].m_model_matrix = glm::scale(g_state.enemies[0].m_model_matrix, glm::vec3(0.5f, 0.5f, 1.0f));

    // ---- ENEMY 2
    g_state.enemies[1].set_entity_type(ENEMIES);
    g_state.enemies[1].m_texture_id = enemy_texture_id;
    g_state.enemies[1].set_ai_type(GUARD);
    g_state.enemies[1].set_ai_state(PATROL);
    g_state.enemies[1].set_position(glm::vec3(9.0f, 0.0f, 0.0f));
    g_state.enemies[1].set_movement(glm::vec3(0.0f));
    g_state.enemies[1].set_speed(1.0f);
    g_state.enemies[1].set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));

    // ---- ENEMY 3
    g_state.enemies[2].set_entity_type(ENEMIES);
    g_state.enemies[2].m_texture_id = enemy_texture_id;
    g_state.enemies[2].set_ai_type(GUARD);
    g_state.enemies[2].set_ai_state(JUMPER);
    g_state.enemies[2].set_position(glm::vec3(13.0f, 0.0f, 0.0f));
    g_state.enemies[2].set_movement(glm::vec3(0.0f));
    g_state.enemies[2].set_speed(1.0f);
    g_state.enemies[2].set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));


    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

    g_state.bgm = Mix_LoadMUS(BGM_FILEPATH);
    Mix_PlayMusic(g_state.bgm, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME/4.0f);
    g_state.jump_sfx = Mix_LoadWAV(JUMP_SFX_FILEPATH);

    // ————— BLENDING ————— //
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input()
{
    g_state.player->set_movement(glm::vec3(0.0f));

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type) {
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            m_game_is_running = false;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_q:
                // Quit the game with a keystroke
                m_game_is_running = false;
                break;

            case SDLK_SPACE:
                // Jump
                if (g_state.player->m_collided_bottom)
                {
                    g_state.player->m_is_jumping = true;
                    Mix_PlayChannel(-1, g_state.jump_sfx, 0);
                }
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
        g_state.player->m_movement.x = -1.0f;
        g_state.player->m_animation_indices = g_state.player->m_walking[g_state.player->LEFT];
    }
    else if (key_state[SDL_SCANCODE_RIGHT])
    {
        g_state.player->m_movement.x = 1.0f;
        g_state.player->m_animation_indices = g_state.player->m_walking[g_state.player->RIGHT];
    }

    // This makes sure that the player can't move faster diagonally
    if (glm::length(g_state.player->m_movement) > 1.0f)
    {
        g_state.player->m_movement = glm::normalize(g_state.player->m_movement);
    }
}


void update()
{
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - m_previous_ticks;
    m_previous_ticks = ticks;

    delta_time += m_accumulator;

    if (delta_time < FIXED_TIMESTEP)
    {
        m_accumulator = delta_time;
        return;
    }

    while (delta_time >= FIXED_TIMESTEP)
    {
        
        if (g_state.player->get_position().y <= -4 && g_state.player->get_position().x <= 0) {
            g_state.player->isWinner = 1;
        }
        if (g_state.player->get_position().y <= -4 && g_state.player->get_position().x > 0) {
            g_state.player->isWinner = 1;
        }
        {
            g_state.player->update(FIXED_TIMESTEP, g_state.player, NULL, 0, g_state.map);  
        }
        for (int i = 0; i < ENEMY_COUNT; i++) {
            g_state.enemies[i].update(FIXED_TIMESTEP, g_state.player, g_state.enemies, 3, g_state.map);
            
        }
        if ( g_state.player->check_collision(&g_state.enemies[0])) {
            float x_distance = fabs(g_state.player->get_position().x - g_state.enemies[0].get_position().x);
            float y_distance = fabs(g_state.player->get_position().y - g_state.enemies[0].get_position().y);
            if (g_state.player->get_velocity().y < 0) {
                g_state.enemies[0].deactivate();
                defeated += 1;
            }
            else {
                g_state.player->isWinner = 1;
            }
           // float x_overlap = fabs(x_distance - (g_state.player->get_width() / 2.0f) - (g_state.enemies[0].get_width()) / 2.0f); 
        }

        if (g_state.player->check_collision(&g_state.enemies[1])) {
            float x_distance = fabs(g_state.player->get_position().x - g_state.enemies[1].get_position().x);
            float y_distance = fabs(g_state.player->get_position().y - g_state.enemies[1].get_position().y);
            if (g_state.player->get_velocity().y < 0) {
                g_state.enemies[1].deactivate();
                defeated += 1;
            }
            else {
                g_state.player->isWinner = 1;
            }
            // float x_overlap = fabs(x_distance - (g_state.player->get_width() / 2.0f) - (g_state.enemies[0].get_width()) / 2.0f); 
        }

        if (g_state.player->check_collision(&g_state.enemies[2])) {
            float x_distance = fabs(g_state.player->get_position().x - g_state.enemies[2].get_position().x);
            float y_distance = fabs(g_state.player->get_position().y - g_state.enemies[2].get_position().y);
            if (g_state.player->get_velocity().y < 0) {
                g_state.enemies[2].deactivate();
                defeated += 1;
            }
            else {
                g_state.player->isWinner = 1;
            }
            // float x_overlap = fabs(x_distance - (g_state.player->get_width() / 2.0f) - (g_state.enemies[0].get_width()) / 2.0f); 
        }

        delta_time -= FIXED_TIMESTEP;
    }
    m_accumulator = delta_time;

    m_view_matrix = glm::mat4(1.0f);
    m_view_matrix = glm::translate(m_view_matrix, glm::vec3(-g_state.player->get_position().x, 0.0f, 0.0f));
}

const int FONTBANK_SIZE = 16;
const int FONTBANK_WIDTH = 16;
const int FONTBANK_HEIGHT = 16;

void DrawText(ShaderProgram* program, GLuint font_texture_id, std::string text, float screen_size, float spacing, glm::vec3 position)
{
    // Scale the size of the fontbank in the UV-plane
    // We will use this for spacing and positioning
    float width = 1.0f / FONTBANK_WIDTH;
    float height = 1.0f / FONTBANK_HEIGHT;

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
        float u_coordinate = (float)(spritesheet_index % FONTBANK_WIDTH) / FONTBANK_WIDTH;
        float v_coordinate = (float)(spritesheet_index / FONTBANK_HEIGHT) / FONTBANK_HEIGHT;

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

void render()
{
    m_program.SetViewMatrix(m_view_matrix);

    glClear(GL_COLOR_BUFFER_BIT);

    g_state.player->render(&m_program);
    //g_state.enemies->render(&m_program);
    g_state.map->render(&m_program);

    for (int i = 0; i < ENEMY_COUNT; i++) {
        g_state.enemies[i].render(&m_program);
    }

    if (g_state.player->isWinner == 1) {
        DrawText(&m_program, load_texture("assets/font2.png"), "YOU LOSE!", 0.55f, 0.0f, glm::vec3(-3.0f, 3.0f, 0.0f));
        DrawText(&m_program, load_texture("assets/font2.png"), "YOU LOSE|", 0.55f, 0.0f, glm::vec3(5.0f, 3.0f, 0.0f));
        DrawText(&m_program, load_texture("assets/font2.png"), "YOU LOSE!", 0.55f, 0.0f, glm::vec3(12.0f, 3.0f, 0.0f));
        g_state.enemies[0].deactivate();
        g_state.enemies[1].deactivate();
        g_state.enemies[2].deactivate();
    }
    if (defeated == 3) {
        DrawText(&m_program, load_texture("assets/font2.png"), "YOU WIN!", 0.55f, 0.0f, glm::vec3(-3.0f, 3.0f, 0.0f));
        DrawText(&m_program, load_texture("assets/font2.png"), "YOU WIN!", 0.55f, 0.0f, glm::vec3(5.0f, 3.0f, 0.0f));
        DrawText(&m_program, load_texture("assets/font2.png"), "YOU WIN!", 0.55f, 0.0f, glm::vec3(12.0f, 3.0f, 0.0f));
    }

    SDL_GL_SwapWindow(m_display_window);
}

void shutdown()
{
    SDL_Quit();

    delete[] g_state.enemies;
    delete    g_state.player;
    delete    g_state.map;
    Mix_FreeChunk(g_state.jump_sfx);
    Mix_FreeMusic(g_state.bgm);
}

// ————— GAME LOOP ————— //
int main(int argc, char* argv[])
{
    initialise();

    while (m_game_is_running)
    {
        process_input();
        update();
        render();
    }

    shutdown();
    return 0;
}