/**
* Author: Kelly Dong
* Assignment: Alien Shooter
* Date due: 2023-08-10, 1:00pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/
#include "Level2.h"
#include "Utility.h"
//include "main.cpp"

Level2::~Level2()
{
    Mix_FreeChunk(m_state.jump_sfx);
    Mix_FreeMusic(m_state.bgm);
}

void Level2::initialise()
{
    m_state.next_scene_id = -1;
    m_state.current_scene_id = 0;
    m_state.player = new Entity();
    m_state.bullet = new Entity();
    m_state.player->deactivate();
    m_state.bullet->deactivate();

    glClearColor(0.0f, 0.0f, 0.2f, 1.0f);

    /**
     BGM and SFX
     */
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

    m_state.bgm = Mix_LoadMUS("assets/space_jazz.mp3");
    Mix_PlayMusic(m_state.bgm, -1);
    Mix_VolumeMusic(100.0f);
    //m_state.jump_sfx = Mix_LoadMUS("assets/boing.mp3");
    m_state.jump_sfx = Mix_LoadWAV("assets/jump.wav");
}

void Level2::update(float delta_time)
{

}

void Level2::render(ShaderProgram* program)
{

    Utility::draw_text(program, Utility::load_texture("assets/font2.png"), "END", 0.25f, 0.00f, glm::vec3(0.0f, 0.0f, 0.0f));
    if (end_game == 3) {
        Utility::draw_text(program, Utility::load_texture("assets/font2.png"), "YOU WIN!", 0.75f, 0.00f, glm::vec3(-2.0f, -3.0f, 0.0f));
    }
    if (end_game == 4) {
        Utility::draw_text(program, Utility::load_texture("assets/font2.png"), "YOU LOSE!", 0.75f, 0.00f, glm::vec3(-2.0f, -3.0f, 0.0f));

    }
}