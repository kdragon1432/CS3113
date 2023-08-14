#include "LevelA.h"
#include "Utility.h"
//include "main.cpp"

#define LEVEL_WIDTH 46
#define LEVEL_HEIGHT 8
#define ENEMY_COUNT 11


unsigned int LEVELA_DATA[] =
{   //level1                                                        //level2                                                //level3
    34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 00, 00, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 00, 00, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34,
    34, 00, 00, 00, 00, 00, 34, 00, 00, 00, 00, 00, 00, 34, 00, 00, 34, 00, 00, 00, 00, 00, 00, 00, 00, 00, 34, 34, 00, 34, 00, 00, 34, 00, 00, 00, 00, 34, 00, 00, 34, 00, 00, 00, 00, 34,
    34, 00, 00, 00, 00, 00, 34, 00, 00, 00, 00, 00, 00, 34, 34, 34, 34, 34, 34, 00, 00, 00, 00, 00, 00, 00, 34, 00, 00, 34, 34, 34, 34, 00, 34, 00, 00, 34, 00, 00, 34, 00, 00, 00, 00, 34,
    34, 00, 00, 00, 00, 00, 34, 00, 00, 34, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 34, 00, 00, 00, 34, 00, 00, 00, 00, 00, 00, 00, 34, 00, 00, 34, 00, 00, 34, 00, 00, 34, 00, 00,
    34, 00, 00, 00, 00, 00, 34, 00, 00, 34, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 34, 00, 00, 00, 00, 34, 00, 00, 34, 34, 34, 34, 34, 00, 00, 00, 34, 00, 00, 00, 00, 34, 34, 00, 00,
    34, 00, 00, 00, 00, 00, 00, 00, 00, 34, 00, 00, 00, 34, 34, 34, 34, 00, 00, 00, 00, 00, 34, 00, 00, 00, 00, 00, 34, 34, 34, 34, 34, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 34, 34, 34,
    34, 00, 00, 00, 00, 00, 00, 00, 00, 34, 00, 00, 00, 34, 00, 00, 34, 00, 00, 00, 00, 00, 34, 00, 00, 00, 00, 34, 34, 34, 00, 00, 34, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 34,
    34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 00, 00, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 00, 00, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34
};

LevelA::~LevelA()
{
    delete [] m_state.enemies;
    delete    m_state.bullet;
    delete    m_state.player;
    delete    m_state.map;
    Mix_FreeChunk(m_state.jump_sfx);
    Mix_FreeMusic(m_state.bgm);
}

void LevelA::initialise()
{
    m_state.next_scene_id = -1;
    m_state.current_scene_id = 1;
    glClearColor(0.0f, 0.0f, 0.2f, 1.0f);
    
    GLuint map_texture_id = Utility::load_texture("assets/tilemap.png");
    m_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVELA_DATA, map_texture_id, 1.0f, 16, 7);
    
    // Code from main.cpp's initialise()
    /**
     George's Stuff
     */
    // Existing
    m_state.player = new Entity();
    m_state.player->activate();
    m_state.player->set_entity_type(PLAYER);
    m_state.player->set_position(glm::vec3(2.0f, -4.5f, 0.0f));
    m_state.player->set_movement(glm::vec3(0.0f));
    m_state.player->set_speed(1.5f);
    m_state.player->set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
    m_state.player->m_texture_id = Utility::load_texture("assets/Character_005.png");

    // Walking
    m_state.player->m_walking[m_state.player->LEFT] = new int[4] { 4, 5, 6, 7 };
    m_state.player->m_walking[m_state.player->RIGHT] = new int[4] { 8, 9, 10, 11 };
    m_state.player->m_walking[m_state.player->UP] = new int[4] { 12, 13, 14, 15 };
    m_state.player->m_walking[m_state.player->DOWN] = new int[4] { 0, 1, 2, 3 };

    m_state.player->m_animation_indices = m_state.player->m_walking[m_state.player->RIGHT];  // start George looking left
    m_state.player->m_animation_frames = 4;
    m_state.player->m_animation_index = 0;
    m_state.player->m_animation_time = 0.0f;
    m_state.player->m_animation_cols = 4;
    m_state.player->m_animation_rows = 4;
    m_state.player->set_height(0.8f);
    m_state.player->set_width(0.8f);

    // Jumping
    m_state.player->m_jumping_power = 8.0f;
    m_state.bullet = new Entity();
    m_state.bullet->set_entity_type(BULLET);
    m_state.bullet->set_position(glm::vec3(-m_state.player->get_position().x, m_state.player->get_position().y, 0.0f));
    m_state.bullet->set_movement(glm::vec3(0.0f));
    m_state.bullet->set_speed(2.5f);
    m_state.bullet->set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
    m_state.bullet->m_texture_id = Utility::load_texture("assets/bullet.png");
    m_state.bullet->deactivate();

    GLuint boost_texture_id = Utility::load_texture("assets/ball.png");

    m_state.boost = new Entity[2];
    m_state.boost[0].set_entity_type(BOOST);
    m_state.boost[0].m_texture_id = boost_texture_id;
    m_state.boost[0].set_position(glm::vec3(2.0f, -2.0f, 0.0f));
    m_state.boost[0].set_movement(glm::vec3(0.0f));
    m_state.boost[0].set_speed(1.5f);
    m_state.boost[0].set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));



    m_state.boost[1].set_entity_type(BOOST);
    m_state.boost[1].m_texture_id = boost_texture_id;
    m_state.boost[1].set_position(glm::vec3(4.0f, -2.0f, 0.0f));
    m_state.boost[1].set_movement(glm::vec3(0.0f));
    m_state.boost[1].set_speed(1.5f);
    m_state.boost[1].set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));


    GLuint enemy_texture_id = Utility::load_texture("assets/shipYellow.png");

    
    // ---- ENEMY 1---
    m_state.enemies = new Entity[ENEMY_COUNT];
    m_state.enemies[0].set_entity_type(ENEMIES);
    m_state.enemies[0].m_texture_id = enemy_texture_id;
    m_state.enemies[0].set_ai_type(GUARD);
    m_state.enemies[0].set_ai_state(WALKING);
    m_state.enemies[0].set_position(glm::vec3(5.0f, -4.0f, 0.0f));
    m_state.enemies[0].set_movement(glm::vec3(0.0f));
    m_state.enemies[0].set_speed(1.5f);
    m_state.enemies[0].set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));

    // ---- ENEMY 2
    m_state.enemies[1].set_entity_type(ENEMIES);
    m_state.enemies[1].m_texture_id = enemy_texture_id;
    m_state.enemies[1].set_ai_type(GUARD);
    m_state.enemies[1].set_ai_state(IDLE);
    m_state.enemies[1].set_position(glm::vec3(10.0f, -4.0f, 0.0f));
    m_state.enemies[1].set_movement(glm::vec3(0.0f));
    m_state.enemies[1].set_speed(1.5f);
    m_state.enemies[1].set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));

    // ---- ENEMY 3
    m_state.enemies[2].set_entity_type(ENEMIES);
    m_state.enemies[2].m_texture_id = enemy_texture_id;
    m_state.enemies[2].set_ai_type(GUARD);
    m_state.enemies[2].set_ai_state(IDLE);
    m_state.enemies[2].set_position(glm::vec3(17.0f, -1.0f, 0.0f)); //tunnel 1
    m_state.enemies[2].set_movement(glm::vec3(0.0f));
    m_state.enemies[2].set_speed(2.5f);
    m_state.enemies[2].set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));


    // ---- ENEMY 4
    m_state.enemies[3].set_entity_type(ENEMIES);
    m_state.enemies[3].m_texture_id = enemy_texture_id;
    m_state.enemies[3].set_ai_type(GUARD);
    m_state.enemies[3].set_ai_state(IDLE);
    m_state.enemies[3].set_position(glm::vec3(20.0f, -6.0f, 0.0f));
    m_state.enemies[3].set_movement(glm::vec3(0.0f));
    m_state.enemies[3].set_speed(2.5f);
    m_state.enemies[3].set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));



    // ---- ENEMY 5
    m_state.enemies[4].set_entity_type(ENEMIES);
    m_state.enemies[4].m_texture_id = enemy_texture_id;
    m_state.enemies[4].set_ai_type(GUARD);
    m_state.enemies[4].set_ai_state(IDLE);
    m_state.enemies[4].set_position(glm::vec3(22.0f, -4.0f, 0.0f));
    m_state.enemies[4].set_movement(glm::vec3(0.0f));
    m_state.enemies[4].set_speed(4.0f);
    m_state.enemies[4].set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));


    // ---- ENEMY 6
    m_state.enemies[5].set_entity_type(ENEMIES);
    m_state.enemies[5].m_texture_id = enemy_texture_id;
    m_state.enemies[5].set_ai_type(GUARD);
    m_state.enemies[5].set_ai_state(IDLE);
    m_state.enemies[5].set_position(glm::vec3(28.0f, -2.0f, 0.0f)); // tunnel 2
    m_state.enemies[5].set_movement(glm::vec3(0.0f));
    m_state.enemies[5].set_speed(2.5f);
    m_state.enemies[5].set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));


    // ---- ENEMY 7
    m_state.enemies[6].set_entity_type(ENEMIES);
    m_state.enemies[6].m_texture_id = enemy_texture_id;
    m_state.enemies[6].set_ai_type(GUARD);
    m_state.enemies[6].set_ai_state(IDLE);
    m_state.enemies[6].set_position(glm::vec3(35.0f, -6.0f, 0.0f));
    m_state.enemies[6].set_movement(glm::vec3(0.0f));
    m_state.enemies[6].set_speed(3.5f);
    m_state.enemies[6].set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));

    // ---- ENEMY 8
    m_state.enemies[7].set_entity_type(ENEMIES);
    m_state.enemies[7].m_texture_id = enemy_texture_id;
    m_state.enemies[7].set_ai_type(GUARD);
    m_state.enemies[7].set_ai_state(IDLE);
    m_state.enemies[7].set_position(glm::vec3(36.0f, -5.0f, 0.0f));
    m_state.enemies[7].set_movement(glm::vec3(0.0f));
    m_state.enemies[7].set_speed(3.5f);
    m_state.enemies[7].set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));

    // ---- ENEMY 8
    m_state.enemies[8].set_entity_type(ENEMIES);
    m_state.enemies[8].m_texture_id = enemy_texture_id;
    m_state.enemies[8].set_ai_type(GUARD);
    m_state.enemies[8].set_ai_state(IDLE);
    m_state.enemies[8].set_position(glm::vec3(38.0f, -2.0f, 0.0f));
    m_state.enemies[8].set_movement(glm::vec3(0.0f));
    m_state.enemies[8].set_speed(3.5f);
    m_state.enemies[8].set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));

    // ---- ENEMY 9
    m_state.enemies[9].set_entity_type(ENEMIES);
    m_state.enemies[9].m_texture_id = enemy_texture_id;
    m_state.enemies[9].set_ai_type(GUARD);
    m_state.enemies[9].set_ai_state(IDLE);
    m_state.enemies[9].set_position(glm::vec3(43.0f, -3.5f, 0.0f)); 
    m_state.enemies[9].set_movement(glm::vec3(0.0f));
    m_state.enemies[9].set_speed(3.0f);
    m_state.enemies[9].set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));

    // ---- ENEMY 9
    m_state.enemies[10].set_entity_type(ENEMIES);
    m_state.enemies[10].m_texture_id = enemy_texture_id;
    m_state.enemies[10].set_ai_type(GUARD);
    m_state.enemies[10].set_ai_state(IDLE);
    m_state.enemies[10].set_position(glm::vec3(44.0f, -6.0f, 0.0f));
    m_state.enemies[10].set_movement(glm::vec3(0.0f));
    m_state.enemies[10].set_speed(6.0f);
    m_state.enemies[10].set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
    
    
    /**
     BGM and SFX
     */
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    
    m_state.bgm = Mix_LoadMUS("assets/space_jazz.mp3");
    Mix_PlayMusic(m_state.bgm, -1);
    Mix_VolumeMusic(100.0f);
    m_state.jump_sfx = Mix_LoadWAV("assets/jump.wav");
}

void LevelA::update(float delta_time)
{
    {
        m_state.player->update(delta_time, m_state.player, NULL, 0, m_state.map);
        m_state.bullet->update(delta_time, m_state.bullet, NULL, 0, m_state.map);
        m_state.boost[0].update(delta_time, m_state.player, m_state.boost, 2, m_state.map);
        m_state.boost[1].update(delta_time, m_state.player, m_state.boost, 2, m_state.map);
    }
    for (int i = 0; i < ENEMY_COUNT; i++) {
        m_state.enemies[i].update(delta_time, m_state.player, m_state.enemies, 11, m_state.map);

    }
    for (int i = 0; i < ENEMY_COUNT; i++) {
        if (m_state.bullet->check_collision(&m_state.enemies[i])) {
            m_state.enemies[i].deactivate();
            m_state.bullet->deactivate();
        }
        if (m_state.player->check_collision(&m_state.enemies[i])) {
            lives -= 1;
            m_state.enemies[i].deactivate();
        }
    }
    if (m_state.player->check_collision(&m_state.boost[0])) {
        m_state.player->set_speed(2.5f);
        m_state.boost[0].deactivate();
        m_state.boost[1].deactivate();
    }
    if (m_state.player->check_collision(&m_state.boost[1])) {
        m_state.bullet->set_speed(4.5f);
        m_state.boost[0].deactivate();
        m_state.boost[1].deactivate();
    }
    if (m_state.player->get_position().x > 46.0f) {
        end_game = 1;
    }
}

void LevelA::render(ShaderProgram *program)
{
    m_state.map->render(program);
    m_state.player->render(program);
    m_state.bullet->render(program);
    m_state.boost[0].render(program);
    m_state.boost[1].render(program);
    for (int i = 0; i < ENEMY_COUNT; i++) {
        m_state.enemies[i].render(program);
    }
    Utility::draw_text(program, Utility::load_texture("assets/font2.png"), "LEVEL A", 0.25f, 0.00f, glm::vec3(1.0f, -0.5f, 0.0f));
    Utility::draw_text(program, Utility::load_texture("assets/font2.png"), "LEVEL B", 0.25f, 0.00f, glm::vec3(17.0f, -0.5f, 0.0f));
    Utility::draw_text(program, Utility::load_texture("assets/font2.png"), "LEVEL C", 0.25f, 0.00f, glm::vec3(33.0f, -0.5f, 0.0f));
    if (lives == 3) {
        Utility::draw_text(program, Utility::load_texture("assets/font2.png"), "LIFE: 3", 0.25f, 0.00f, glm::vec3(m_state.player->get_position().x - .5, -1.0f, 0.0f));
    }
    if (lives == 2) {
        Utility::draw_text(program, Utility::load_texture("assets/font2.png"), "LIFE: 2", 0.25f, 0.00f, glm::vec3(m_state.player->get_position().x - .5, -1.0f, 0.0f));
    }
    if (lives == 1) {
        Utility::draw_text(program, Utility::load_texture("assets/font2.png"), "LIFE: 1", 0.25f, 0.00f, glm::vec3(m_state.player->get_position().x - .5, -1.0f, 0.0f));
    }
    if (lives == 0) {
        end_game = 2;
    }
    Utility::draw_text(program, Utility::load_texture("assets/font2.png"), "GOAL!", 0.25f, 0.00f, glm::vec3(46.0f, -3.5f, 0.0f));
}
