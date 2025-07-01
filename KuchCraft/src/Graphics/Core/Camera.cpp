#include "kcpch.h"
#include "Graphics/Core/Camera.h"

namespace KuchCraft {

	void Camera::UpdateTransform(const glm::vec3& position, const glm::vec3& rotation)
	{
		m_Position = position;
		m_Rotation = rotation;

		glm::vec3 front;
		front.x = glm::cos(m_Rotation.x) * glm::cos(m_Rotation.y);
		front.y = glm::sin(m_Rotation.y);
		front.z = glm::sin(m_Rotation.x) * glm::cos(m_Rotation.y);

		m_Front = glm::normalize(front);
		m_Right = glm::normalize(glm::cross(m_Front, m_Up));

		UpdateView();
	}

	void Camera::ConfigureProjection(float fov, float aspectRatio, float nearClip, float farClip)
	{
		m_Fov         = fov;
		m_AspectRatio = aspectRatio;
		m_NearClip    = nearClip;
		m_FarClip     = farClip;
		UpdateProjection();
	}

	void Camera::UpdateProjection()
	{
		m_Projection     = glm::perspective(m_Fov, m_AspectRatio, m_NearClip, m_FarClip);
		m_ViewProjection = m_Projection * m_View;
	}

	void Camera::UpdateView()
	{
		m_View = glm::lookAt(m_Position, m_Position + GetForwardDirection(), GetUpDirection());
		m_ViewProjection = m_Projection * m_View;
	}

}
