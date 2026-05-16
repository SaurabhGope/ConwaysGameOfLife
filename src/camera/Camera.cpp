#include "camera/Camera.hpp"

#include <cmath>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

void Camera::SetPerspective(float fovYRadians, float aspect, float zNear, float zFar)
{
    m_fovYRadians = fovYRadians;
    m_aspect = aspect;
    m_zNear = zNear;
    m_zFar = zFar;
}

void Camera::SetPosition(const glm::vec3& position)
{
    m_position = position;
}

void Camera::SetYawPitch(float yawRadians, float pitchRadians)
{
    m_yawRadians = yawRadians;
    m_pitchRadians = pitchRadians;
}

const glm::vec3& Camera::Position() const
{
    return m_position;
}

glm::mat4 Camera::ViewMatrix() const
{
    glm::vec3 forward;
    forward.x = std::cos(m_pitchRadians) * std::cos(m_yawRadians);
    forward.y = std::sin(m_pitchRadians);
    forward.z = std::cos(m_pitchRadians) * std::sin(m_yawRadians);

    return glm::lookAt(m_position, m_position + glm::normalize(forward), glm::vec3(0.0f, 1.0f, 0.0f));
}

glm::mat4 Camera::ProjectionMatrix() const
{
    return glm::perspective(m_fovYRadians, m_aspect, m_zNear, m_zFar);
}

glm::mat4 Camera::ViewProjectionMatrix() const
{
    return ProjectionMatrix() * ViewMatrix();
}
