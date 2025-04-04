#pragma once
#include "../types/types.h"
#include "../window.h"

#include <glm/gtc/matrix_transform.hpp>

class Camera
{
private:
    glm::vec3 _position{ 0.0f, 1.5f, 30.0f };
    const glm::vec3 _origin{_position};
    glm::vec3 _direction{ glm::normalize(-_position) };
    glm::vec3 _forward{glm::vec3(0.0f, 0.0f, -1.0f)};
    glm::vec3 _up{ 0.0f, 1.0f, 0.0f };
    glm::vec3 _right{ glm::normalize(glm::cross(_up, _direction)) };

    float _speed = 3.0f;

    void CalculateDirection(Window* window);
    void CalculateKeyboard(Window* window);

    glm::vec3 _movementDirection{0.0f};

    Matrices _matrices;
public:
    Camera(const Camera&)            = delete;
    Camera& operator=(const Camera&) = delete;
    Camera();
    ~Camera();
    void Update(Window* window);
    float GetSpeed()                       const    { return _speed;       }
    glm::vec3 GetUp()                      const    { return _up;          }
    void SetSpeed(float speed)                      { _speed = speed;      }
    const glm::vec3 GetOrigin()            const    { return _origin;      } 
    const Matrices& GetMVP()                        { return _matrices;    }
    void SetMatrices(Matrices& matr)                { _matrices = matr;    }
	const glm::vec3 GetPosition()          const    { return _position;    }
    const glm::vec3 GetMovementDirection() const    { return _movementDirection;   }
    void SetPosition(glm::vec3 newPos)              { _position = newPos;  }

};


// Purpose: as with the grow of the project would need a lot of different cameras it is
// better to make one common handle for them
namespace SamuraiCameras
{
    extern const int32_t g_cameraCount;
    extern Camera g_mainCamera;
    extern Camera g_editorCamera;
};