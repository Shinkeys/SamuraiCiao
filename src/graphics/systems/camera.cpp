#include "../../headers/systems/camera.h"
#include <algorithm>

Camera  SamuraiCameras::g_mainCamera(CameraType::CAMERA_TYPE_MAIN);
Camera  SamuraiCameras::g_editorCamera(CameraType::CAMERA_TYPE_EDIT);
Camera* SamuraiCameras::g_activeCamera = &g_mainCamera;
const int32_t SamuraiCameras::g_cameraCount = 2;




Camera::Camera(CameraType type)
{
	_behaviour.cameraType = type;
	switch(type)
	{
	case CameraType::CAMERA_TYPE_MAIN:
		_behaviour.canMoveFreely   = true;
		_behaviour.canMoveVertical = false;
		break;
	
	case CameraType::CAMERA_TYPE_EDIT:
		_behaviour.canMoveFreely   = false;
		_behaviour.canMoveVertical = true;
		break;

	case CameraType::CAMERA_TYPE_NONE:
		_behaviour.canMoveFreely   = false;
		_behaviour.canMoveVertical = false;
		break;

	default: 
		std::cout << "Unrecognized camera type\n";
		break;
	}

}


Camera::~Camera()
{

}

void Camera::Update(Window* window)
{
	CalculateDirection(window);
	CalculateButtons(window);

	_matrices.view = glm::lookAt(_position, _position + _direction, _up);
}

void Camera::CalculateButtons(Window* window)
{
	if(window == nullptr)
	{
		std::cerr << "Window pointer is null in camera calculate buttons method\n";
		return;
	}

	static float lastFrame = 0.0f;
	const float currentFrame = glfwGetTime();
	const float deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	// In the editor mode it's possible to move only with RMB + WASD
	if(!window->GetMouseState().mouseRight && _behaviour.cameraType == CameraType::CAMERA_TYPE_EDIT)
		return;
	else if(window->GetMouseState().mouseRight && _behaviour.cameraType == CameraType::CAMERA_TYPE_EDIT)
	{
		if (window->GetKeysState().right)
		{
			_position += glm::normalize(_right) * deltaTime * _speed;
		}
		if (window->GetKeysState().left)
		{
			_position -= glm::normalize(_right) * deltaTime * _speed;
		}
		if (window->GetKeysState().front)
		{
			_position += _forward * deltaTime * _speed;
		}
		if (window->GetKeysState().back)
		{
			_position -= _forward * deltaTime * _speed;
		}
	}

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

	if(window->GetKeysState().jump)
	{
		_needToJump = true;
	}

	_movementDirection = movDirStack;
}
void Camera::CalculateDirection(Window* window)
{
	if(window == nullptr)
	{
		std::cerr << "Window pointer is null in camera calculate direction method\n";
		return;
	}

	// If not in editor mode - return
	if(window->GetKeysState().sceneEditor && !window->GetMouseState().mouseMiddle)
		return;


	const float sensitivity = 0.05f;

	float rotateX = window->GetMouseState().displacementX;
	float rotateY = window->GetMouseState().displacementY;

	window->ResetMouse();

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

	if(_behaviour.canMoveVertical)
		_forward = glm::normalize(_direction);
	else 
		_forward = glm::normalize(glm::cross(_up, _right));
}