#include "../../headers/systems/camera.h"
#include <algorithm>

Matrices Camera::_matrices;

Camera::Camera()
{
}


Camera::~Camera()
{

}

void Camera::Update(Window* window)
{
	CalculateDirection(window);
	CalculateKeyboard(window);
	Camera::_matrices.view = glm::lookAt(_position, _position + _direction, _up);
}

void Camera::CalculateKeyboard(Window* window)
{
	if(window == nullptr)
	{
		std::cerr << "Window pointer is null in camera calculate keyboard method\n";
		return;
	}

	static float lastFrame = 0.0f;
	const float currentFrame = glfwGetTime();
	const float deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
	if (window->GetKeysState().right)
	{
		_position += glm::normalize(_right) * deltaTime;
	}
	if (window->GetKeysState().left)
	{
		_position -= glm::normalize(_right) * deltaTime;
	}
	if (window->GetKeysState().front)
	{
		_position += _forward * deltaTime;
	}
	if (window->GetKeysState().back)
	{
		_position -= _forward * deltaTime;
	}
	_position.y = 5.0f;
}
void Camera::CalculateDirection(Window* window)
{
	if(window == nullptr)
	{
		std::cerr << "Window pointer is null in camera calculate direction method\n";
		return;
	}

	const float sensitivity = 0.05f;

	float rotateX = window->GetLastMouseOffset().x;
	float rotateY = window->GetLastMouseOffset().y;

	static float yaw = -90.0f; // camera initially looks in x direction, but character position forward(z)
	static float pitch = 0.0f;
	if (rotateX || rotateY)
	{
		yaw += rotateX * sensitivity;
		pitch += rotateY * sensitivity;

		const float maxPitch = 89.0f;
		pitch = std::clamp(pitch, -maxPitch, maxPitch);
	}

	_direction.x = std::cos(glm::radians(yaw)) * std::cos(glm::radians(pitch));
	_direction.y = std::sin(glm::radians(-pitch));
    _direction.z = std::sin(glm::radians(yaw)) * std::cos(glm::radians(pitch));
	_right = glm::normalize(glm::cross(_direction, _up));

	_forward = glm::normalize(glm::cross(_up, _right));

	window->ResetMouse();
}