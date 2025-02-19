#pragma once
#include "../types/types.h"
#include "../window.h"

#include <glm/gtc/matrix_transform.hpp>

class Camera
{
private:
    glm::vec3 _position{ 0.0f, 5.0f, 5.0f };
    glm::vec3 _direction{ glm::normalize(-_position) };
    glm::vec3 _forward{glm::vec3(0.0f, 0.0f, -1.0f)};
    glm::vec3 _up{ 0.0f, 1.0f, 0.0f };
    glm::vec3 _right{ glm::normalize(glm::cross(_up, _direction)) };
    void CalculateDirection(Window* window);
    void CalculateKeyboard(Window* window);

    Matrices _matrices;
public:
    Camera(const Camera&) = delete;
    Camera& operator=(const Camera&) = delete;
    Camera();
    ~Camera();
    void Update(Window* window);
    const Matrices& GetMVP() const { return _matrices;}
    void SetMatrices(Matrices& matr) { _matrices = matr;}
	glm::vec3 GetPosition() { return _position; }

};