#include "kcpch.h"
#include "Graphics/RendererLayer.h"

#include <imgui.h>

namespace KuchCraft {

	RendererLayer::RendererLayer(const Ref<Renderer>& renderer)
		: Layer("RendererLayer", LayerType::Debug), m_Renderer(renderer)
	{

	}

	RendererLayer::~RendererLayer()
	{
	}

	void RendererLayer::OnAttach()
	{
	}

	void RendererLayer::OnDetach()
	{
	}

	void RendererLayer::OnUpdate(Timestep ts)
	{
		if (Input::IsKeyPressed(KeyCode::F10))
		{
			if (m_State == LayerState::Active)
				SetState(LayerState::NotVisible);
			else if (m_State == LayerState::NotVisible)
				SetState(LayerState::Active);
		}
	}

	void RendererLayer::OnTick(const Timestep ts)
	{
	}

	void RendererLayer::OnRender()
	{
	}

	void RendererLayer::OnImGuiRender()
	{
		ImGui::Begin("Render Debug Tools");

		constexpr float margin = 6.0f;
		
		ImGui::Indent(margin);
		if (ImGui::CollapsingHeader("Statistics##RendererLayer", ImGuiTreeNodeFlags_DefaultOpen))
		{
			const auto& stats = m_Renderer->GetStats();

			ImGui::Text("Draw calls: %d", stats.DrawCalls);
			ImGui::Text("Vertices: %d", stats.Vertices);
			ImGui::Text("Quads: %d", stats.Quads);
		}
		ImGui::Unindent(margin);
		
		ImGui::Indent(margin);
		if (ImGui::CollapsingHeader("Shaders##RendererLayer"))
		{
			std::string shaderLibrarySeparatorText = "Library: " + m_Renderer->m_ShaderLibrary.GetName();
			ImGui::SeparatorText(shaderLibrarySeparatorText.c_str());

			if (ImGui::Button("Reload All##RendererLayer", ImVec2(ImGui::GetContentRegionAvail().x, 0.0f)))
			{
				m_Renderer->m_ShaderLibrary.ReloadAll();
			}

			ImGui::SeparatorText("Shader list");

			ImGui::BeginChild("ShaderList", ImVec2(0.0f, 150.0f), true);
			const auto& shaders = m_Renderer->m_ShaderLibrary.GetShaders();
			int index = 0;
			for (const auto& [name, shader] : shaders)
			{
				bool isSelectd = (m_ShadersInfo.Selected == index);
				if (ImGui::Selectable(name.c_str(), isSelectd))
				{
					m_ShadersInfo.Selected     = index;
					m_ShadersInfo.SelectedName = name;
				}

				if (isSelectd)
					ImGui::SetItemDefaultFocus();

				index++;
			}
			ImGui::EndChild();

			if (m_ShadersInfo.Selected != -1)
			{
				const auto& shader = m_Renderer->m_ShaderLibrary.Get(m_ShadersInfo.SelectedName);
				if (shader)
				{
					ImGui::SeparatorText("Shader info");
					ImGui::Text("Name: %s", shader->GetName());
					ImGui::Text("Path: %s", shader->GetPath().string().c_str());
					ImGui::Text("RendererID: %d", shader->GetRendererID());

					ImGui::SeparatorText("");

					ImGui::Indent(margin);

					if (ImGui::CollapsingHeader("Raw source##RendererLayer"))
						ImGui::TextWrapped("%s", shader->GetSource().c_str());
					if (ImGui::CollapsingHeader("Shader Code##RendererLayer"))
					{
						const auto& shaderSources = shader->GetShaderSources();
						for (const auto& [type, source] : shaderSources)
						{
							ImGui::Indent(margin);

							std::string label = std::string(ToString(type)) + "##RendererLayer_RawSource";
							if (ImGui::CollapsingHeader(label.c_str()))
								ImGui::TextWrapped("%s", source.c_str());

							ImGui::Unindent(margin);
						}
					}

					if (ImGui::CollapsingHeader("Local substitutions##RendererLayer"))
					{
						const auto& localSubstitutions = shader->GetLocalSubstitutions();
						if (!localSubstitutions.empty())
						{
							if (ImGui::BeginTable("TwoColumnTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
							{
								ImGui::TableSetupColumn("Key", ImGuiTableColumnFlags_WidthStretch);
								ImGui::TableSetupColumn("Vaule", ImGuiTableColumnFlags_WidthStretch);
								ImGui::TableHeadersRow();
								for (const auto& [key, value] : localSubstitutions)
								{
									ImGui::TableNextRow();
									ImGui::TableSetColumnIndex(0);
									ImGui::TextUnformatted(key.c_str());
									ImGui::TableSetColumnIndex(1);
									ImGui::TextUnformatted(value.c_str());
								}
								ImGui::EndTable();
							}
						}
						else
						{
							ImGui::Text("No data");
						}
					}

					if (ImGui::CollapsingHeader("Variables##RendererLayer"))
					{
						const auto& variables = shader->GetVariables();
						if (!variables.empty())
						{
							for (const auto& [type, vars] : variables)
							{
								ImGui::Indent(margin);

								std::string label = std::string(ToString(type)) + "##RendererLayer_Variables";
								if (ImGui::CollapsingHeader(label.c_str()))
								{
									for (const auto& var : vars)
									{
										if (var.Location != -1)
											ImGui::TextWrapped("Location %d - %s: %s - %s", var.Location, var.Name, std::string(ToString(var.Qualifier)), std::string(ToString(var.Type)));
										else
											ImGui::TextWrapped("%s: %s - %s", var.Name, std::string(ToString(var.Qualifier)), std::string(ToString(var.Type)));
									}
								}
								ImGui::Unindent(margin);
							}
						}
						else
						{
							ImGui::Text("No data");
						}
					}

					if (ImGui::CollapsingHeader("Uniform Blocks##RendererLayer"))
					{
						const auto& uniformBlocks = shader->GetUniformBlocks();
						if (!uniformBlocks.empty())
						{
							for (const auto& [type, blocks] : uniformBlocks)
							{
								ImGui::Indent(margin);

								std::string label = std::string(ToString(type)) + "##RendererLayer_UniformBlocks";
								if (ImGui::CollapsingHeader(label.c_str()))
								{
									for (const auto& block : blocks)
									{
										ImGui::Text("Name: %s", block.Name);
										ImGui::Text("Layout: %s", block.Layout);
										ImGui::Text("Layout: %d", block.Binding);

										ImGui::SeparatorText("variables");
										ImGui::Indent(margin);
										for (const auto& var : block.Members)
										{
											ImGui::TextWrapped("%s - %s", var.Name, std::string(ToString(var.Qualifier)));
										}
										ImGui::Unindent(margin);
									}
								}
								ImGui::Unindent(margin);
							}
						}
						else
						{
							ImGui::Text("No data");
						}
					}

					if (ImGui::CollapsingHeader("UniformLocations##RendererLayer"))
					{
						const auto& locations = shader->GetUniformLocations();
						if (!locations.empty())
						{
							for (const auto& [name, location] : locations)
							{ 
								ImGui::Text("%d - %s", location, name);
							}
						}
						else
						{
							ImGui::Text("No data");
						}
					}

					ImGui::Unindent(margin);
				}
				else
				{
					m_ShadersInfo.Selected = -1;
				}	
			}
		}
		ImGui::Unindent(margin);

		ImGui::End();
	}

	void RendererLayer::OnApplicationEvent(ApplicationEvent& e)
	{

	}
}