/**
* Author: Kelly Dong
* Assignment: Platformer
* Date due: 2023-08-01, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/
#pragma once
#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Utility.h"
#include "Entity.h"
#include "Map.h"

struct GameState
{
    Map    *map;
    Entity *player;
    Entity *enemies;
    
    Mix_Music *bgm;
    Mix_Chunk *jump_sfx;
    
    int current_scene_id;
    int next_scene_id;
};

class Scene {
public:
    int m_number_of_enemies = 1;
    
    GameState m_state;
    
    virtual void initialise() = 0;
    virtual void update(float delta_time) = 0;
    virtual void render(ShaderProgram *program) = 0;
    
    GameState const get_state() const { return m_state; }
};
