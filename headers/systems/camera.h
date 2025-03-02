#pragma once
#include "../types/types.h"
#include "../window.h"

#include <glm/gtc/matrix_transform.hpp>

class Camera
{
private:
    glm::vec3 _position{ 0.0f, 2.5f, 25.0f };
    const glm::vec3 _origin{_position};
    glm::vec3 _direction{ glm::normalize(-_position) };
    glm::vec3 _forward{glm::vec3(0.0f, 0.0f, -1.0f)};
    glm::vec3 _up{ 0.0f, 1.0f, 0.0f };
    glm::vec3 _right{ glm::normalize(glm::cross(_up, _direction)) };
    void CalculateDirection(Window* window);
    void CalculateKeyboard(Window* window);

    static Matrices _matrices;
public:
    Camera(const Camera&) = delete;
    Camera& operator=(const Camera&) = delete;
    Camera();
    ~Camera();
    void Update(Window* window);
    const glm::vec3 GetOrigin() const { return _origin;} 
    static const Matrices& GetMVP() { return _matrices;}
    void SetMatrices(Matrices& matr) { _matrices = matr;}
	const glm::vec3 GetPosition() const { return _position; }

};