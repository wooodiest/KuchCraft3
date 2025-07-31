layout (std140, binding = #value(ENVIRONMENT_UNIFORM_BUFFER_BINDING)) uniform EnvironmentUniformBuffer
{
	mat4 u_ViewProjection;
	mat4 u_OrthoProjection;
};