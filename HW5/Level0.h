/**
* Author: Kelly Dong
* Assignment: Platformer
* Date due: 2023-08-01, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/
#include "Scene.h"

class Level0 : public Scene {
public:
    ~Level0();
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;

};