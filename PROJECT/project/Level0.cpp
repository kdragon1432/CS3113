#include "Level0.h"
#include "Utility.h"
//include "main.cpp"

Level0::~Level0()
{
    Mix_FreeChunk(m_state.jump_sfx);
    Mix_FreeMusic(m_state.bgm);
}

void Level0::initialise()
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

void Level0::update(float delta_time)
{

}

void Level0::render(ShaderProgram* program)
{

    Utility::draw_text(program, Utility::load_texture("assets/font2.png"), "ALIEN SHOOTER", 0.50f, 0.00f, glm::vec3(-3.0f, -0.5f, 0.0f));
    Utility::draw_text(program, Utility::load_texture("assets/font2.png"), "GOAL: REACH THE END AND DEFEAT ALL ENEMIES", 0.25f, 0.00f, glm::vec3(-5.0f, -2.5f, 0.0f));
    Utility::draw_text(program, Utility::load_texture("assets/font2.png"), "USE ARROW KEYS TO MOVE", 0.25f, 0.00f, glm::vec3(-3.0f, -3.5f, 0.0f));
    Utility::draw_text(program, Utility::load_texture("assets/font2.png"), "USE SPACE BAR TO SHOOT", 0.25f, 0.00f, glm::vec3(-3.0f, -4.5f, 0.0f));
    Utility::draw_text(program, Utility::load_texture("assets/font2.png"), "PRESS ENTER TO START", 0.50f, 0.00f, glm::vec3(-4.5f, -6.5f, 0.0f));
}