#pragma once
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

namespace tomway
{
    class camera_controller
    {
    public:
        camera_controller(glm::vec3 initial_pos, float initial_vert_rot, float initial_hor_rot);
        glm::mat4 get_projection_transform(uint32_t width, uint32_t height) const;
        glm::mat4 get_view_transform() const;
        void reset();
        void update(float delta);
    private:
        glm::vec3 _pos, _pos_initial;
        glm::vec3 _right = {}, _fwd = {}, _up = {};
        float _hor_rot, _hor_rot_initial;
        float _vert_rot, _vert_rot_initial;
        float _near_plane = 0.1f;
        float _far_plane = 1000.0f;
        float _fov = 45.0f;
        float _speed = 4.0f;
    };
}