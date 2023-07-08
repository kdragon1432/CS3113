/**
* Author: Kelly Dong
* Assignment: Lunar Lander
* Date due: 2023-07-07, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

class Entity
{
private:
    int* m_animation_right = NULL, // move to the right
        * m_animation_left = NULL, // move to the left
        * m_animation_up = NULL, // move upwards
        * m_animation_down = NULL; // move downwards

    glm::vec3 m_position;
public:
    // ————— STATIC VARIABLES ————— //
    static const int SECONDS_PER_FRAME = 4;
    static const int LEFT = 0,
        RIGHT = 1,
        UP = 2,
        DOWN = 3;

    // ————— ANIMATION ————— //
    int** m_walking = new int* [4]
    {
        m_animation_left,
            m_animation_right,
            m_animation_up,
            m_animation_down
    };

    int m_animation_frames = 0,
        m_animation_index = 0,
        m_animation_cols = 0,
        m_animation_rows = 0;

    int* m_animation_indices = NULL;
    float m_animation_time = 0.0f;

    // ————— TRANSFORMATIONS ————— //
    float     m_speed;
    glm::vec3 m_movement;
    glm::vec3 m_velocity;
    glm::vec3 m_acceleration;
    glm::mat4 m_model_matrix;

    GLuint    m_texture_id;

    float width = 1;
    float height = 1;

    bool isActive = true;
    int isWinner = 0;

    bool collidedTop = false;
    bool collidedBottom = false;
    bool collidedLeft = false;
    bool collidedRight = false;


    // ————— METHODS ————— //
    Entity();
    ~Entity();

    bool CheckCollision(Entity* other);
    void CheckCollisionsY(Entity* objects, int objectCount);
    void CheckCollisionsX(Entity* objects, int objectCount);
    void draw_sprite_from_texture_atlas(ShaderProgram* program, GLuint texture_id, int index);
    void update(float delta_time);
    void render(ShaderProgram* program);

    // ————— GETTERS ————— //
    glm::vec3 const get_position() const { return m_position; };
    glm::vec3 const get_movement() const { return m_movement; };

    // ————— SETTERS ————— //
    void const set_position(glm::vec3 new_position) { m_position = new_position; };
    void const set_movement(glm::vec3 new_movement) { m_movement = new_movement; };
};
