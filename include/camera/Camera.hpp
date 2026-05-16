#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

class Camera
{
public:
    void SetPerspective(float fovYRadians, float aspect, float zNear, float zFar);
    void SetPosition(const glm::vec3& position);
    void SetYawPitch(float yawRadians, float pitchRadians);

    const glm::vec3& Position() const;
    glm::mat4 ViewMatrix() const;
    glm::mat4 ProjectionMatrix() const;
    glm::mat4 ViewProjectionMatrix() const;

private:
    glm::vec3 m_position{0.0f, 0.0f, 5.0f};
    float m_yawRadians = 0.0f;
    float m_pitchRadians = 0.0f;

    float m_fovYRadians = 1.0471976f;
    float m_aspect = 16.0f / 9.0f;
    float m_zNear = 0.1f;
    float m_zFar = 1000.0f;
};
