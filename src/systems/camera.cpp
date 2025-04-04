#include "../../headers/systems/camera.h"
#include <algorithm>

Camera SamuraiCameras::g_mainCamera;
Camera SamuraiCameras::g_editorCamera;
const int32_t SamuraiCameras::g_cameraCount = 2;



Camera::Camera()
{
}


Camera::~Camera()
{

}

void Camera::Update(Window* window)
{
	if(!window->GetKeysState().showImgui)
	{
		CalculateDirection(window);
		CalculateKeyboard(window);
	}
	_matrices.view = glm::lookAt(_position, _position + _direction, _up);
}

void Camera::CalculateKeyboard(Window* window)
{
	if(window == nullptr)
	{
		std::cerr << "Window pointer is null in camera calculate keyboard method\n";
		return;
	}

	// static float lastFrame = 0.0f;
	// const float currentFrame = glfwGetTime();
	// const float deltaTime = currentFrame - lastFrame;
	// lastFrame = currentFrame;
	// if (window->GetKeysState().right)
	// {
	// 	_position += glm::normalize(_right) * deltaTime * _speed;
	// }
	// if (window->GetKeysState().left)
	// {
	// 	_position -= glm::normalize(_right) * deltaTime * _speed;
	// }
	// if (window->GetKeysState().front)
	// {
	// 	_position += _forward * deltaTime * _speed;
	// }
	// if (window->GetKeysState().back)
	// {
	// 	_position -= _forward * deltaTime * _speed;
	// }
	// _position.y = 2.5f;


	// To check //

	glm::vec3 movDirStack = glm::vec3(0.0f);
	if (window->GetKeysState().right)
	{
		movDirStack += glm::normalize(_right);
	}
	if (window->GetKeysState().left)
	{
		movDirStack -= glm::normalize(_right);
	}
	if (window->GetKeysState().front)
	{
		movDirStack += _forward;
	}
	if (window->GetKeysState().back)
	{
		movDirStack -= _forward;
	}

	movDirStack.y = 0.0f;

	_movementDirection = movDirStack;
}
void Camera::CalculateDirection(Window* window)
{
	if(window == nullptr)
	{
		std::cerr << "Window pointer is null in camera calculate direction method\n";
		return;
	}

	const float sensitivity = 0.05f;

	float rotateX = window->GetMousePositions().x;
	float rotateY = window->GetMousePositions().y;

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