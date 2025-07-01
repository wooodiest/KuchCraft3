#pragma once

namespace KuchCraft {

	class Camera
	{
	public:
		Camera()
		{
			UpdateView();
			UpdateProjection();
		}

		Camera(float fov, float aspectRatio, float nearClip, float farClip)
		{
			UpdateView();
			ConfigureProjection(fov, aspectRatio, nearClip, farClip);
		}

		~Camera() = default;

		void UpdateTransform(const glm::vec3& position, const glm::vec3& rotation);
		void SetPosition(const glm::vec3& position) { UpdateTransform(position, m_Rotation); }
		void SetRotation(const glm::vec3& rotation) { UpdateTransform(m_Position, rotation); }

		void ConfigureProjection(float fov, float aspectRatio, float nearClip, float farClip);
		void SetFov(float fov)                 { m_Fov = fov;                 UpdateProjection(); }
		void SetAspectRatio(float aspectRatio) { m_AspectRatio = aspectRatio; UpdateProjection(); }
		void SetNearClip(float nearClip)       { m_NearClip = nearClip;       UpdateProjection(); }
		void SetFarClip(float farClip)         { m_FarClip  = farClip;        UpdateProjection(); }

		const glm::mat4& GetView()           const { return m_View;           }
		const glm::mat4& GetProjection()     const { return m_Projection;     }
		const glm::mat4& GetViewProjection() const { return m_ViewProjection; }

		const glm::vec3& GetPosition() const { return m_Position; }
		const glm::vec3& GetRotation() const { return m_Rotation; }

		const glm::vec3& GetUpDirection()      const { return m_Up;    }
		const glm::vec3& GetRightDirection()   const { return m_Right; }
		const glm::vec3& GetForwardDirection() const { return m_Front; }

		float GetFov()         const { return m_Fov;         }
		float GetAspectRatio() const { return m_AspectRatio; }
		float GetNearClip()    const { return m_NearClip;    }
		float GetFarClip()     const { return m_FarClip;     }

	private:
		void UpdateProjection();
		void UpdateView();

	private:
		float m_Fov = glm::radians(45.0f);
		float m_AspectRatio = 16.0f / 9.0f;

		float m_NearClip = 0.01f;
		float m_FarClip  = 1000.0f;

		glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_Rotation = { 0.0f, 0.0f, 0.0f };

		glm::mat4 m_Projection = { 1.0f };
		glm::mat4 m_View       = { 1.0f };
		glm::mat4 m_ViewProjection = { 1.0f };

		glm::vec3 m_Up   { 0.0f, 1.0f, 0.0f };
		glm::vec3 m_Right{ 1.0f, 0.0f, 0.0f };
		glm::vec3 m_Front{ 0.0f, 0.0f, -1.0f };
	};

}