#include "SceneHierarchyPanel.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <misc/cpp/imgui_stdlib.h>

#include <glm/gtc/type_ptr.hpp>

#include <cstring>

namespace Shadow
{
    SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& context)
    {
        SetScene(context);
    }

    void SceneHierarchyPanel::SetScene(const Ref<Scene>& context)
    {
        m_Scene = context;
        m_SelectionEntity = {};
    }

    void SceneHierarchyPanel::OnImGuiRender()
    {
        ImGui::Begin("Scene Hierarchy");
        if (m_Scene)
        {
            m_Scene->m_Registry.each([&](auto entityID)
                {
                    Entity entity{ entityID , m_Scene.get() };
                    DrawEntityNode(entity);
                });

            if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
                m_SelectionEntity = {};

            if (ImGui::BeginPopupContextWindow(0, 1, false))
            {
                if (ImGui::MenuItem("Create Empty Entity"))
                    m_Scene->CreateEntity("Empty Entity");
                ImGui::EndPopup();
            }
        }
        ImGui::End();

        ImGui::Begin("Properties");
        if (m_SelectionEntity)
        {
            DrawComponents(m_SelectionEntity);
        }
        ImGui::End();
    }

    void SceneHierarchyPanel::SetSelectedEntity(Entity entity)
    {
        m_SelectionEntity = entity;
    }

    void SceneHierarchyPanel::DrawEntityNode(Entity entity)
    {
        auto& tag = entity.GetComponent<TagComponent>().Tag;
        // ImGuiTreeNodeFlags_Selected 可以在ImGui的UI界面中高亮显示某个树状节点，以表示该节点当前被选中。这通常用于处理用户交互或者UI交互效果。
        // ImGuiTreeNodeFlags_OpenOnArrow 使得用户可以通过点击箭头图标来展开节点，而不仅仅是点击节点标题来实现展开操作。这可以提供更灵活和直观的用户体验。
        ImGuiTreeNodeFlags flags = ((m_SelectionEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
        flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
        bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());
        if (ImGui::IsItemClicked())
        {
            m_SelectionEntity = entity;
        }

        bool entityDeleted = false;
        if (ImGui::BeginPopupContextItem())
        {
            if (ImGui::MenuItem("Delete Entity"))
            {
                entityDeleted = true;
            }

            ImGui::EndPopup();
        }

        if (opened)
        {
            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
            bool opened = ImGui::TreeNodeEx((void*)9817239, flags, tag.c_str());
            if (opened)
            {
                ImGui::TreePop();
            }
            ImGui::TreePop();
        }

        if (entityDeleted)
        {
            m_Scene->DestroyEntity(entity);
            if (m_SelectionEntity == entity)
            {
                m_SelectionEntity = {};
            }
        }
    }

    template<typename T, typename UIFunction>
    static void DrawComponent(const std::string& name, Entity entity, UIFunction uiFunction)
    {
        // ImGuiTreeNodeFlags_DefaultOpen: 这个标志位表示默认情况下节点是展开状态的，即当节点第一次创建时，默认是打开状态。
        // ImGuiTreeNodeFlags_Framed: 这个标志位指示节点将被放置在一个框架中，使得节点在界面上显示为一个带边框的区域。
        // ImGuiTreeNodeFlags_SpanAvailWidth: 这个标志位表示节点的宽度将尽可能占据可用空间，使得节点在水平方向上扩展以填充可用的宽度。
        // ImGuiTreeNodeFlags_AllowItemOverlap : 这个标志位表示节点内的项可以重叠显示，即允许节点内部的元素在视觉上有一定的重叠。
        // ImGuiTreeNodeFlags_FramePadding: 这个标志位表示为节点使用与框架相同的内边距值，确保节点内的内容与边框有适当的间距。
        const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen
            | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth
            | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;

        if (entity.HasComponent<T>())
        {
            auto& component = entity.GetComponent<T>();
            // 行宽行高获取
            ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

            // 分割线
            ImGui::Separator();

            // 内边距设置为 4 单位
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
            // 创建一个可展开/折叠的树状节点，并将节点的开放状态存储在布尔变量open中
            // 生成一个类型T的哈希码，并将其转换为void*指针。这个哈希码用作树状节点的唯一标识，以便ImGui能够正确地管理和显示节点的展开/折叠状态。
            bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
            ImGui::PopStyleVar();

            // 填充高度
            float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
            // 填充剩下的空间，只留下高度一半的空间，给+号（因为同一行的TreeNodeEx设置为可以折叠，所以这边可以这样做）
            ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
            if (ImGui::Button("+", ImVec2{ lineHeight, lineHeight }))
            {
                ImGui::OpenPopup("ComponentSettings");
            }

            bool removeComponent = false;
            if (ImGui::BeginPopup("ComponentSettings"))
            {
                if (ImGui::MenuItem("Remove component"))
                {
                    removeComponent = true;
                }

                ImGui::EndPopup();
            }

            if (open)
            {
                uiFunction(component);
                ImGui::TreePop();
            }

            if (removeComponent)
                entity.RemoveComponent<T>();
        }
    }

    void SceneHierarchyPanel::DrawComponents(Entity entity)
    {
        if (entity.HasComponent<TagComponent>())
        {
            auto& tag = entity.GetComponent<TagComponent>().Tag;

            char buffer[256];
            memset(buffer, 0, sizeof(buffer));
            strncpy_s(buffer, sizeof(buffer), tag.c_str(), sizeof(buffer));
            if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
            {
                tag = std::string(buffer);
            }
        }

        ImGui::SameLine();
        ImGui::PushItemWidth(-1);

        if (ImGui::Button("Add Component"))
        {
            ImGui::OpenPopup("AddComponent");
        }

        if (ImGui::BeginPopup("AddComponent"))
        {
            DisplayAddComponentEntry<SpriteRendererComponent>("Sprite Renderer");
            DisplayAddComponentEntry<CircleRendererComponent>("Circle Renderer");
            DisplayAddComponentEntry<CameraComponent>("Camera");
            DisplayAddComponentEntry<TextComponent>("Text Component");
            DisplayAddComponentEntry<Rigidbody2DComponent>("Rigidbody 2D");
            DisplayAddComponentEntry<BoxCollider2DComponent>("Box Collider 2D");
            DisplayAddComponentEntry<CircleCollider2DComponent>("Circle Collider 2D");

            ImGui::EndPopup();
        }

        ImGui::PopItemWidth();

        DrawComponent<TransformComponent>("Transform", entity, [](auto& component)
            {
                DrawVec3Control("Translation", component.Translation);
                glm::vec3 rotation = glm::degrees(component.Rotation);
                DrawVec3Control("Rotation", rotation);
                component.Rotation = glm::radians(rotation);
                DrawVec3Control("Scale", component.Scale, 1.0f);
            });

        DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [](auto& component)
            {
                ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));

                ImGui::Button("Texture", ImVec2(100.0f, 0.0f));
                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
                    {
                        const wchar_t* path = (const wchar_t*)payload->Data;
                        std::filesystem::path texturePath(path);
                        Ref<Texture2D> texture = Texture2D::Create(texturePath.string());
                        if (texture->IsLoaded())
                            component.Texture = texture;
                        else
                            SD_WARN("Could not load texture {0}", texturePath.filename().string());
                    }
                    ImGui::EndDragDropTarget();
                }

                ImGui::DragFloat("Tiling Factor", &component.TilingFactor, 0.1f, 0.0f, 100.0f);
            });

        DrawComponent<CircleRendererComponent>("Circle Renderer", entity, [](auto& component)
            {
                ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));
                ImGui::DragFloat("Thickness", &component.Thickness, 0.025f, 0.0f, 1.0f);
                ImGui::DragFloat("Fade", &component.Fade, 0.00025f, 0.0f, 1.0f);
            });

        DrawComponent<CameraComponent>("Camera", entity, [](auto& component)
            {
                auto& camera = component.Camera;

                ImGui::Checkbox("Primary", &component.Primary);

                const char* projectionTypeStrings[] = { "Perspective", "Orthographic" };
                const char* currentProjectionTypeString = projectionTypeStrings[(int)camera.GetProjectionType()];
                if (ImGui::BeginCombo("Projection", currentProjectionTypeString))
                {
                    for (int i = 0; i < 2; i++)
                    {
                        bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];
                        if (ImGui::Selectable(projectionTypeStrings[i], isSelected))
                        {
                            currentProjectionTypeString = projectionTypeStrings[i];
                            camera.SetProjectionType((SceneCamera::ProjectionType)i);
                        }

                        if (isSelected)
                            ImGui::SetItemDefaultFocus();
                    }

                    ImGui::EndCombo();
                }

                if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
                {
                    float perspectiveVerticalFov = glm::degrees(camera.GetPerspectiveVerticalFOV());
                    if (ImGui::DragFloat("Vertical FOV", &perspectiveVerticalFov))
                        camera.SetPerspectiveVerticalFOV(glm::radians(perspectiveVerticalFov));

                    float perspectiveNear = camera.GetPerspectiveNearClip();
                    if (ImGui::DragFloat("Near", &perspectiveNear))
                        camera.SetPerspectiveNearClip(perspectiveNear);

                    float perspectiveFar = camera.GetPerspectiveFarClip();
                    if (ImGui::DragFloat("Far", &perspectiveFar))
                        camera.SetPerspectiveFarClip(perspectiveFar);
                }

                if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
                {
                    float orthoSize = camera.GetOrthographicSize();
                    if (ImGui::DragFloat("Size", &orthoSize))
                        camera.SetOrthographicSize(orthoSize);

                    float orthoNear = camera.GetOrthographicNearClip();
                    if (ImGui::DragFloat("Near", &orthoNear))
                        camera.SetOrthographicNearClip(orthoNear);

                    float orthoFar = camera.GetOrthographicFarClip();
                    if (ImGui::DragFloat("Far", &orthoFar))
                        camera.SetOrthographicFarClip(orthoFar);

                    ImGui::Checkbox("Fixed Aspect Ratio", &component.FixedAspectRatio);
                }
            });

        DrawComponent<TextComponent>("Text Renderer", entity, [](auto& component)
            {
                ImGui::InputTextMultiline("Text String", &component.TextString);
                ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));
                ImGui::DragFloat("Kerning", &component.Kerning, 0.025f);
                ImGui::DragFloat("Line Spacing", &component.LineSpacing, 0.025f);
            });

        DrawComponent<Rigidbody2DComponent>("Rigidbody 2D", entity, [](auto& component)
            {
                const char* bodyTypeStrings[] = { "Static", "Dynamic", "Kinematic" };
                const char* currentBodyTypeString = bodyTypeStrings[(int)component.Type];
                if (ImGui::BeginCombo("Body Type", currentBodyTypeString))
                {
                    for (int i = 0; i < 2; i++)
                    {
                        bool isSelected = currentBodyTypeString == bodyTypeStrings[i];
                        if (ImGui::Selectable(bodyTypeStrings[i], isSelected))
                        {
                            currentBodyTypeString = bodyTypeStrings[i];
                            component.Type = (Rigidbody2DComponent::BodyType)i;
                        }

                        if (isSelected)
                            ImGui::SetItemDefaultFocus();
                    }

                    ImGui::EndCombo();
                }

                ImGui::Checkbox("Fixed Rotation", &component.FixedRotation);
            });

        DrawComponent<BoxCollider2DComponent>("Box Collider 2D", entity, [](auto& component)
            {
                ImGui::DragFloat2("Offset", glm::value_ptr(component.Offset));
                ImGui::DragFloat2("Size", glm::value_ptr(component.Size));
                ImGui::DragFloat("Density", &component.Density, 0.01f, 0.0f, 1.0f);
                ImGui::DragFloat("Friction", &component.Friction, 0.01f, 0.0f, 1.0f);
                ImGui::DragFloat("Restitution", &component.Restitution, 0.01f, 0.0f, 1.0f);
                ImGui::DragFloat("Restitution Threshold", &component.RestitutionThreshold, 0.01f, 0.0f);
            });

        DrawComponent<CircleCollider2DComponent>("Circle Collider 2D", entity, [](auto& component)
            {
                ImGui::DragFloat2("Offset", glm::value_ptr(component.Offset));
                ImGui::DragFloat("Radius", &component.Radius);
                ImGui::DragFloat("Density", &component.Density, 0.01f, 0.0f, 1.0f);
                ImGui::DragFloat("Friction", &component.Friction, 0.01f, 0.0f, 1.0f);
                ImGui::DragFloat("Restitution", &component.Restitution, 0.01f, 0.0f, 1.0f);
                ImGui::DragFloat("Restitution Threshold", &component.RestitutionThreshold, 0.01f, 0.0f);
            });
    }

    template<typename T>
    void SceneHierarchyPanel::DisplayAddComponentEntry(const std::string& entryName) {
        if (!m_SelectionEntity.HasComponent<T>())
        {
            if (ImGui::MenuItem(entryName.c_str()))
            {
                m_SelectionEntity.AddComponent<T>();
                ImGui::CloseCurrentPopup();
            }
        }
    }

    static void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f)
    {
        ImGuiIO& io = ImGui::GetIO();
        auto boldFont = io.Fonts->Fonts[0];

        ImGui::PushID(label.c_str());

        // 双列布局
        ImGui::Columns(2);

        // 设置第一列宽度
        ImGui::SetColumnWidth(0, columnWidth);
        ImGui::Text(label.c_str());

        // 移动光标到下一列
        ImGui::NextColumn();

        // 最近三项非自定义大小的内容自动计算平分空间，使用
        ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
        // 垂直和水平的项目间距设置为0，以此紧凑表示XYZ
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

        // GImGui->Style.FramePadding.y 在 Y 轴方向上的框架内边距的数值。
        float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
        // 长度设置为行高加3
        ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

        // 默认、悬停、激活下的颜色
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
        ImGui::PushFont(boldFont);
        // Button自己设置了大小，所以不听从PushMultiItemsWidths的
        if (ImGui::Button("X", buttonSize))
        {
            values.x = resetValue;
        }
        ImGui::PopFont();
        ImGui::PopStyleColor(3);

        // 在当前行内布局中将光标移动到同一行的起始位置，从而使后续创建的界面元素在同一行内水平排列。这可以帮助实现更加紧凑的布局效果。
        ImGui::SameLine();
        // "##" 是一个技巧，用于隐藏显示标签但实际上仍有一个标签存在。
        ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
        ImGui::PushFont(boldFont);
        if (ImGui::Button("Y", buttonSize))
        {
            values.y = resetValue;
        }
        ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
        ImGui::PushFont(boldFont);
        if (ImGui::Button("Z", buttonSize))
        {
            values.z = resetValue;
        }
        ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();

        ImGui::PopStyleVar();

        ImGui::Columns(1);

        ImGui::PopID();
    }
}