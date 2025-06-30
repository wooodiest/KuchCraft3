layout (std140, binding = #pragma value(ENVIRONMENT_UNIFORM_BUFFER_BINDING)) uniform EnvironmentUniformBuffer
{
	mat4 u_ViewProjection;
	mat4 u_OrthoProjection;
};