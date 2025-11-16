#include "camera.hpp"

Camera::Camera(int width, int height, glm::vec3 position) {
    this->width = width;
    this->height = height;
    this->position = position;
}

void Camera::updateMatrix(float FOVdeg, float nearPlane, float farPlane) {
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);
    view = glm::lookAt(position, position + orientation, up);
    projection = glm::perspective(glm::radians(FOVdeg), (((float)width)/((float)height)), nearPlane, farPlane);

    cameraMatrix = projection * view;
}

void Camera::lookAt(v3 target) {
	// get orientation vector
	this->orientation = glm::normalize(target - position);
}

void Camera::matrix(Shader *shader, const char* uniform) {
    glUniformMatrix4fv(glGetUniformLocation(shader->id, uniform), 1, GL_FALSE, glm::value_ptr(cameraMatrix));
}

void Camera::inputs(GLFWwindow* window)
{
	// Handles key inputs
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
		position += speed * orientation;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		position += speed * -glm::normalize(glm::cross(orientation, up));
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		position += speed * -orientation;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		position += speed * glm::normalize(glm::cross(orientation, up));
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		position += speed * up;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
		position += speed * -up;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		speed = 0.4f;
	}
	else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE) {
		speed = 0.1f;
	}


	// Handles mouse inputs
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		// Hides mouse cursor
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		// Prevents camera from jumping on the first click
		if (firstClick) {
			glfwSetCursorPos(window, (width / 2), (height / 2));
			firstClick = false;
		}

		// Stores the coordinates of the cursor
		double mouseX;
		double mouseY;
		// Fetches the coordinates of the cursor
		glfwGetCursorPos(window, &mouseX, &mouseY);

		// Normalizes and shifts the coordinates of the cursor such that they begin in the middle of the screen
		// and then "transforms" them into degrees 
		float rotX = sensitivity * (float)(mouseY - (height / 2)) / height;
		float rotY = sensitivity * (float)(mouseX - (width / 2)) / width;

		// Calculates upcoming vertical change in the orientation
		glm::vec3 neworientation = glm::rotate(orientation, glm::radians(-rotX), glm::normalize(glm::cross(orientation, up)));

		// Decides whether or not the next vertical orientation is legal or not
		if (abs(glm::angle(neworientation, up) - glm::radians(90.0f)) <= glm::radians(85.0f))
		{
			orientation = neworientation;
		}

		// Rotates the orientation left and right
		orientation = glm::rotate(orientation, glm::radians(-rotY), up);

		// Sets mouse cursor to the middle of the screen so that it doesn't end up roaming around
		glfwSetCursorPos(window, (width / 2), (height / 2));
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
		// Unhides cursor since camera is not looking around anymore
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		// Makes sure the next time the camera looks around it doesn't jump
		firstClick = true;
	}
}