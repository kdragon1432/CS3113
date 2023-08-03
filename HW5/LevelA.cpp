/**
* Author: Kelly Dong
* Assignment: Platformer
* Date due: 2023-08-01, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/
#include "LevelA.h"
#include "Utility.h"
//include "main.cpp"

#define LEVEL_WIDTH 14
#define LEVEL_HEIGHT 8


unsigned int LEVELA_DATA[] =
{
    54, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    54, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    54, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    54, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    54, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    54, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    54, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    54, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18
};

LevelA::~LevelA()
{
    delete [] m_state.enemies;
    delete    m_state.player;
    delete    m_state.map;
    Mix_FreeChunk(m_state.jump_sfx);
    Mix_FreeMusic(m_state.bgm);
}

void LevelA::initialise()
{
    m_state.next_scene_id = -1;
    m_state.current_scene_id = 1;
    
    GLuint map_texture_id = Utility::load_texture("assets/tilemap.png");
    m_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVELA_DATA, map_texture_id, 1.0f, 16, 7);
    
    // Code from main.cpp's initialise()
    /**
     George's Stuff
     */
    // Existing
    m_state.player = new Entity();
    m_state.player->set_entity_type(PLAYER);
    m_state.player->set_position(glm::vec3(4.0f, 0.0f, 0.0f));
    m_state.player->set_movement(glm::vec3(0.0f));
    m_state.player->m_speed = 2.5f;
    m_state.player->set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));
    m_state.player->m_texture_id = Utility::load_texture("assets/Character_005.png");

    m_state.player->m_walking[m_state.player->LEFT] = new int[4] { 4, 5, 7, 7 };
    m_state.player->m_walking[m_state.player->RIGHT] = new int[4] { 8, 9, 10, 11 };
    m_state.player->m_walking[m_state.player->UP] = new int[4] { 12, 13, 14, 15 };
    m_state.player->m_walking[m_state.player->DOWN] = new int[4] { 0, 1, 2, 3 };

    m_state.player->m_animation_indices = m_state.player->m_walking[m_state.player->RIGHT];  // start George looking left
    m_state.player->m_animation_frames = 4;
    m_state.player->m_animation_index  = 0;
    m_state.player->m_animation_time   = 0.0f;
    m_state.player->m_animation_cols   = 4;
    m_state.player->m_animation_rows   = 4;
    m_state.player->set_height(0.8f);
    m_state.player->set_width(0.8f);
    
    // Jumping
    m_state.player->m_jumping_power = 9.0f;
    
    /**
     Enemies' stuff */
    GLuint enemy_texture_id = Utility::load_texture("assets/shipYellow.png");
    
    m_state.enemies = new Entity[1];
    m_state.enemies[0].set_entity_type(ENEMY);
    m_state.enemies[0].set_ai_type(GUARD);
    m_state.enemies[0].set_ai_state(IDLE);
    m_state.enemies[0].m_texture_id = enemy_texture_id;
    m_state.enemies[0].set_position(glm::vec3(9.0f, 12.0f, 0.0f));
    m_state.enemies[0].set_movement(glm::vec3(0.0f));
    m_state.enemies[0].m_speed = 1.0f;
    m_state.enemies[0].set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));
    m_state.enemies[0].activate();
    
    
    /**
     BGM and SFX
     */
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    
    m_state.bgm = Mix_LoadMUS("assets/space_jazz.mp3");
    Mix_PlayMusic(m_state.bgm, -1);
    Mix_VolumeMusic(100.0f);
    //m_state.jump_sfx = Mix_LoadMUS("assets/boing.mp3");
    m_state.jump_sfx = Mix_LoadWAV("assets/bounce1.wav");
}

void LevelA::update(float delta_time)
{
    m_state.player->update(delta_time, m_state.player, NULL, 0, m_state.map);
    m_state.enemies[0].update(delta_time, m_state.player, m_state.enemies, ENEMY_COUNT, m_state.map);
    if (m_state.player->check_collision(&m_state.enemies[0])) {
        m_state.enemies[0].deactivate();
        m_state.player->lost += 1;
       // number_of_lives -= 1;
    }
    
    if (m_state.player->get_position().y < -10.0f) m_state.next_scene_id = 2;
}

void LevelA::render(ShaderProgram *program)
{
    m_state.map->render(program);
    m_state.player->render(program);
    m_state.enemies[0].render(program);
   
    Utility::draw_text(program, Utility::load_texture("assets/font2.png"), "LEVEL A", 0.25f, 0.00f, glm::vec3(1.0f, -0.5f, 0.0f));
    Utility::draw_text(program, Utility::load_texture("assets/font2.png"), "GOAL!", 0.25f, 0.00f, glm::vec3(14.0f, -7.0f, 0.0f));
}
