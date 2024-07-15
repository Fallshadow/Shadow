#include "EditorLayer.h"
#include "Shadow/Scene/SceneSerializer.h"
#include "Shadow/Utils/PlatformUtils.h"
#include "Shadow/Math/Math.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <ImGuizmo.h>

namespace Shadow
{
    EditorLayer::EditorLayer() : Layer("EditorLayer")
    {

    }

    void EditorLayer::OnAttach()
    {
        m_EditorScene = CreateRef<Scene>();
        m_EditorCamera = EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);
        m_ActiveScene = m_EditorScene;

        FrameBufferSpecification fbSpec;
        fbSpec.Attachments = { FrameBufferTextureFormat::RGBA8, FrameBufferTextureFormat::RED_INTEGER, FrameBufferTextureFormat::Depth };
        fbSpec.Width = 1280;
        fbSpec.Height = 720;
        m_FrameBuffer = FrameBuffer::Create(fbSpec);

        if (!OpenProject())
        {
            Application::Get().Close();
        }

        m_IconPlay = Texture2D::Create("Resources/Icons/PlayButton.png");
        m_IconPause = Texture2D::Create("Resources/Icons/PauseButton.png");
        m_IconSimulate = Texture2D::Create("Resources/Icons/SimulateButton.png");
        m_IconStep = Texture2D::Create("Resources/Icons/StepButton.png");
        m_IconStop = Texture2D::Create("Resources/Icons/StopButton.png");
    }

    void EditorLayer::OnDetach()
    {

    }

    void EditorLayer::OnUpdate(TimeStep ts)
    {
        // 随时判断是否 resize
        m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
        if (FrameBufferSpecification spec = m_FrameBuffer->GetSpecification();
            m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && (spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
        {
            m_FrameBuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
            m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
        }

        // Render
        Renderer2D::ResetStats();
        m_FrameBuffer->Bind();
        // 清空屏幕
        RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
        RenderCommand::Clear();
        // 清空帧缓冲
        m_FrameBuffer->ClearAttachment(1, -1);

        switch (m_SceneState)
        {
            case SceneState::Edit:
            {
                m_EditorCamera.OnUpdate(ts);

                m_ActiveScene->OnUpdateEditor(m_EditorCamera);
                break;
            }
            case SceneState::Simulate:
            {
                m_EditorCamera.OnUpdate(ts);

                m_ActiveScene->OnUpdateSimulation(ts, m_EditorCamera);
                break;
            }
            case SceneState::Play:
            {
                m_ActiveScene->OnUpdateRuntime(ts);
                break;
            }
        }

        auto [mx, my] = ImGui::GetMousePos();
        mx -= m_ViewportBounds[0].x;
        my -= m_ViewportBounds[0].y;
        glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
        my = viewportSize.y - my;
        int mouseX = (int)mx;
        int mouseY = (int)my;

        if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y)
        {
            int pixelData = m_FrameBuffer->ReadPixel(1, mouseX, mouseY);
            m_HoveredEntity = pixelData == -1 ? Entity() : Entity((entt::entity)pixelData, m_ActiveScene.get());
        }

        OnOverlayRender();

        m_FrameBuffer->Unbind();
    }

    void EditorLayer::OnImGuiRender()
    {
        // 创建Dock时使用的标记
        ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);

        // ImGuiWindowFlags_MenuBar 指定窗口具有菜单栏。在窗口顶部显示一个菜单栏，用户可以在其中添加自定义菜单项。
        // ImGuiWindowFlags_NoDocking 禁用悬浮
        // ------------------------------------------------------------------------------------------------------
        // ImGuiWindowFlags_NoTitleBar 指定窗口没有标题栏。会隐藏窗口的标题栏，使窗口没有标题文字和拖动条。
        // ImGuiWindowFlags_NoCollapse 指定窗口不可折叠。禁用窗口的折叠功能，用户无法通过双击标题栏来折叠窗口。
        // ImGuiWindowFlags_NoResize 指定窗口不可调整大小。禁用窗口的调整大小功能，使窗口大小固定不变。
        // ImGuiWindowFlags_NoMove 指定窗口不可移动。禁用窗口的移动功能，用户无法拖动窗口改变位置。
        // ------------------------------------------------------------------------------------------------------
        // ImGuiWindowFlags_NoBringToFrontOnFocus 指定窗口在获得焦点时不将其移动到最前。即使用户点击窗口，也不会将窗口移到其他窗口之上。
        // ImGuiWindowFlags_NoNavFocus 指定窗口不获取导航焦点。导航不会将焦点转移到该窗口，用户无法通过键盘导航到该窗口内的控件。
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        // 当我们设置的标记里包含ImGuiDockNodeFlags_PassthruCentralNode时，DockSpace()将渲染我们的背景并处理传递孔，所以我们要求Begin()不渲染背景。
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        // 将下面的主窗口内边距设置为0，即内容紧贴边缘
        // 尝试加宽间距，你会发现，MenuBar就是紧贴边缘的，只有内容边距被加宽
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        // 圆角样式为0，边框大小为0
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        // 使用标记创建主窗口
        // p_open：是一个指向布尔值的指针，用于控制窗口的显示状态。
        // 如果 p_open 不为 NULL，并且 *p_open 为 true，则窗口将保持打开状态，直到用户手动关闭它。
        // 如果 p_open 为 NULL，则窗口没有关闭按钮，并且用户无法手动关闭它。
        bool dockspaceOpen = true;
        ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
        ImGui::PopStyleVar(3);

        // DockSpace
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        // 创建停靠区域，大小自动调整
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

#pragma region MenuBar
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Open Project...", "Ctrl+O"))
                    OpenProject();

                ImGui::Separator();

                if (ImGui::MenuItem("New Scene", "Ctrl+N"))
                    NewScene();

                if (ImGui::MenuItem("Save Scene", "Ctrl+S"))
                    SaveScene();

                if (ImGui::MenuItem("Save Scene As...", "Ctrl+Shift+S"))
                    SaveSceneAs();

                ImGui::Separator();

                if (ImGui::MenuItem("Exit"))
                    Shadow::Application::Get().Close();

                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }
#pragma endregion

        m_SceneHierarchyPanel.OnImGuiRender();
        m_ContentBrowserPanel->OnImGuiRender();

#pragma region Stats
        ImGui::Begin("Stats");
        auto stats = Shadow::Renderer2D::GetStats();
        ImGui::Text("Renderer2D Stats:");
        ImGui::Text("Draw Calls: %d", stats.DrawCalls);
        ImGui::Text("Quads: %d", stats.QuadCount);
        ImGui::Text("Circles: %d", stats.CircleCount);
        ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
        ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
        ImGui::End();
#pragma endregion

#pragma region ViewPort
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
        ImGui::Begin("Viewport");
        auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
        auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
        auto viewportOffset = ImGui::GetWindowPos();
        m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
        m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

        m_ViewportHovered = ImGui::IsWindowHovered();
        Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportHovered);

        // 获取当前内容区域的可用大小，并将其存储在 viewportPanelSize 中
        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
        m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };
        uint64_t textureID = m_FrameBuffer->GetColorAttachmentRendererID();
        // 这段代码利用ImGui的Image函数在界面上展示纹理图像。
        // 参数依次为：纹理的指针（通过 ID 转换得到），图像的尺寸（使用 m_ViewportSize），UV 纹理坐标的起点 {0,1}和终点 {1,0}，分别对应纹理的左上和右下角。
        ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

        // 检测拖入窗口的文件
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
            {
                const wchar_t* path = (const wchar_t*)payload->Data;
                OpenScene(path);
            }
            ImGui::EndDragDropTarget();
        }

        // Gizmos
        Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
        if (selectedEntity && m_GizmoType != -1)
        {
            ImGuizmo::SetOrthographic(false);
            ImGuizmo::SetDrawlist();
            ImGuizmo::SetRect(m_ViewportBounds[0].x, m_ViewportBounds[0].y, m_ViewportBounds[1].x - m_ViewportBounds[0].x, m_ViewportBounds[1].y - m_ViewportBounds[0].y);

            // Runtime camera from entity
            // auto cameraEntity = m_ActiveScene->GetPrimaryCameraEntity();
            // const auto& camera = cameraEntity.GetComponent<CameraComponent>().Camera;
            // const glm::mat4& cameraProjection = camera.GetProjection();
            // glm::mat4 cameraView = glm::inverse(cameraEntity.GetComponent<TransformComponent>().GetTransform());

            // Editor camera
            const glm::mat4& cameraProjection = m_EditorCamera.GetProjection();
            glm::mat4 cameraView = m_EditorCamera.GetViewMatrix();

            // Entity transform
            auto& tc = selectedEntity.GetComponent<TransformComponent>();
            glm::mat4 transform = tc.GetTransform();

            // Snapping
            bool snap = Input::IsKeyPressed(Key::LeftControl);
            float snapValue = 0.5f; // Snap to 0.5m for translation/scale
            // Snap to 45 degrees for rotation
            if (m_GizmoType == ImGuizmo::OPERATION::ROTATE)
                snapValue = 45.0f;

            float snapValues[3] = { snapValue, snapValue, snapValue };

            ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
                (ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform),
                nullptr, snap ? snapValues : nullptr);

            if (ImGuizmo::IsUsing())
            {
                glm::vec3 translation, rotation, scale;
                Math::DecomposeTransform(transform, translation, rotation, scale);

                glm::vec3 deltaRotation = rotation - tc.Rotation;
                tc.Translation = translation;
                tc.Rotation += deltaRotation;
                tc.Scale = scale;
            }
        }

        ImGui::End();
        ImGui::PopStyleVar();
#pragma endregion

        UI_Toolbar();

        ImGui::End();
    }

    void EditorLayer::OnEvent(Event& e)
    {
        if (m_SceneState == SceneState::Edit)
        {
            m_EditorCamera.OnEvent(e);
        }

        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<KeyPressedEvent>(SD_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
        dispatcher.Dispatch<MouseButtonPressedEvent>(SD_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
    }

#pragma region Menu

    bool EditorLayer::OpenProject()
    {
        std::string filepath = FileDialogs::OpenFile("Hazel Project (*.hproj)\0*.hproj\0");
        if (filepath.empty())
            return false;

        OpenProject(filepath);
        return true;
    }

    void EditorLayer::OpenProject(const std::filesystem::path& path)
    {
        if (Project::Load(path))
        {
            auto startScenePath = Project::GetAssetFileSystemPath(Project::GetActive()->GetConfig().StartScene);
            OpenScene(startScenePath);
            m_ContentBrowserPanel = CreateScope<ContentBrowserPanel>();
        }
    }

    void EditorLayer::OpenScene(const std::filesystem::path& path)
    {
        if (path.extension().string() != ".hazel")
        {
            SD_WARN("Could not load {0} - not a scene file", path.filename().string());
            return;
        }

        Ref<Scene> newScene = CreateRef<Scene>();
        SceneSerializer ss(newScene);
        if (ss.Deserialize(path.string()))
        {
            m_EditorScene = newScene;
            m_SceneHierarchyPanel.SetScene(m_EditorScene);

            m_ActiveScene = m_EditorScene;
            m_EditorScenePath = path;
        }
    }

    void EditorLayer::NewScene()
    {
        m_EditorScene = CreateRef<Scene>();
        m_ActiveScene = m_EditorScene;
        m_EditorScenePath = std::filesystem::path();

        m_SceneHierarchyPanel.SetScene(m_ActiveScene);
    }

    void EditorLayer::SaveScene()
    {
        if (!m_EditorScenePath.empty())
            SerializeScene(m_ActiveScene, m_EditorScenePath);
        else
            SaveSceneAs();
    }

    void EditorLayer::SaveSceneAs()
    {
        // 用于显示保存文件对话框的函数，用户可以选择文件路径和文件名进行保存。
        std::string filepath = FileDialogs::SaveFile("Hazel Scene (*.hazel)\0*.hazel\0");
        if (!filepath.empty())
        {
            SerializeScene(m_ActiveScene, filepath);
            m_EditorScenePath = filepath;
        }
    }

    void EditorLayer::SerializeScene(Ref<Scene> scene, const std::filesystem::path& path)
    {
        SceneSerializer serializer(scene);
        serializer.Serialize(path.string());
    }

#pragma endregion

#pragma region ToolBar

    void EditorLayer::UI_Toolbar()
    {
        // 窗口上下边距为 0 ，左右边距为 2 ，这样窗口的上下更加紧凑
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
        // 窗口内容间距为 0
        ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        auto& colors = ImGui::GetStyle().Colors;
        const auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
        const auto& buttonActive = colors[ImGuiCol_ButtonActive];
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));

        // ImGuiWindowFlags_NoDecoration: 这个标记表示窗口不带有装饰，比如标题栏、边框等，使窗口看起来更简洁。
        // ImGuiWindowFlags_NoScrollbar: 这个标记表示禁用滚动条，如果窗口内容太多而无法显示完全，将不会有滚动条出现。
        // ImGuiWindowFlags_NoScrollWithMouse : 这个标记表示当鼠标滚轮滚动时，窗口内容不会滚动，而是传递给父级窗口（如果有的话）来处理滚动事件。
        ImGui::Begin("##toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

        bool toolbarEnabled = (bool)m_ActiveScene;

        ImVec4 tintColor = ImVec4(1, 1, 1, 1);
        if (!toolbarEnabled)
            tintColor.w = 0.5f;

        float size = ImGui::GetWindowHeight() - 4.0f;
        ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));

        if (m_SceneState == SceneState::Edit)
        {
            if (ImGui::ImageButton((ImTextureID)(uint64_t)m_IconPlay->GetRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), ImVec4(1, 1, 1, 1)))
            {
                OnScenePlay();
            }

            ImGui::SameLine();

            if (ImGui::ImageButton((ImTextureID)(uint64_t)m_IconSimulate->GetRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), ImVec4(1, 1, 1, 1)))
            {
                OnSceneSimulate();
            }
        }

        if (m_SceneState == SceneState::Play || m_SceneState == SceneState::Simulate)
        {
            if (ImGui::ImageButton((ImTextureID)(uint64_t)m_IconStop->GetRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), ImVec4(1, 1, 1, 1)))
            {
                OnSceneStop();
            }

            ImGui::SameLine();

            bool isPaused = m_ActiveScene->IsPaused();
            if (ImGui::ImageButton((ImTextureID)(uint64_t)m_IconPause->GetRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), ImVec4(1, 1, 1, 1)))
            {
                m_ActiveScene->SetPaused(!isPaused);
            }

            if (isPaused)
            {
                ImGui::SameLine();
                {
                    if (ImGui::ImageButton((ImTextureID)(uint64_t)m_IconStep->GetRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), ImVec4(1, 1, 1, 1)))
                    {
                        m_ActiveScene->Step();
                    }
                }
            }
        }

        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor(3);
        ImGui::End();
    }

    void EditorLayer::OnScenePlay()
    {
        if (m_SceneState == SceneState::Simulate)
        {
            OnSceneStop();
        }

        m_SceneState = SceneState::Play;
        m_ActiveScene = Scene::Copy(m_EditorScene);
        m_ActiveScene->OnRuntimeStart();
        m_SceneHierarchyPanel.SetScene(m_ActiveScene);
    }

    void EditorLayer::OnSceneSimulate()
    {
        if (m_SceneState == SceneState::Play) 
        {
            OnSceneStop();
        }

        m_SceneState = SceneState::Simulate;
        m_ActiveScene = Scene::Copy(m_EditorScene);
        m_ActiveScene->OnSimulationStart();
        m_SceneHierarchyPanel.SetScene(m_ActiveScene);
    }

    void EditorLayer::OnSceneStop()
    {
        SD_CORE_ASSERT(m_SceneState == SceneState::Play || m_SceneState == SceneState::Simulate);

        if (m_SceneState == SceneState::Play)
        {
            m_ActiveScene->OnRuntimeStop();
        }
        else if (m_SceneState == SceneState::Simulate)
        {
            m_ActiveScene->OnSimulationStop();
        }

        m_SceneState = SceneState::Edit;
        m_ActiveScene = m_EditorScene;
        m_SceneHierarchyPanel.SetScene(m_ActiveScene);
    }

    void EditorLayer::OnScenePause()
    {
        if (m_SceneState == SceneState::Edit)
            return;

        m_ActiveScene->SetPaused(true);
    }

#pragma endregion

    bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
    {
        if (e.IsRepeat())
            return false;

        bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
        bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);
        switch (e.GetKeyCode())
        {
            case Key::N:
            {
                if (control)
                    NewScene();

                break;
            }
            case Key::O:
            {
                if (control)
                    OpenProject();

                break;
            }
            case Key::S:
            {
                if (control)
                {
                    if (shift)
                        SaveSceneAs();
                    else
                        SaveScene();
                }

                break;
            }

            case Key::D:
            {
                if (control)
                    OnDuplicateEntity();
                break;
            }

            case Key::Q:
            {
                if (!ImGuizmo::IsUsing())
                    m_GizmoType = -1;
                break;
            }
            case Key::W:
            {
                if (!ImGuizmo::IsUsing())
                    m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
                break;
            }
            case Key::E:
            {
                if (!ImGuizmo::IsUsing())
                    m_GizmoType = ImGuizmo::OPERATION::ROTATE;
                break;
            }
            case Key::R:
            {
                if (!ImGuizmo::IsUsing())
                    m_GizmoType = ImGuizmo::OPERATION::SCALE;
                break;
            }
            case Key::Delete:
            {
                if (Application::Get().GetImGuiLayer()->GetActiveWidgetID() == 0)
                {
                    Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
                    if (selectedEntity)
                    {
                        m_SceneHierarchyPanel.SetSelectedEntity({});
                        m_ActiveScene->DestroyEntity(selectedEntity);
                    }
                }
                break;
            }
        }

        return false;
    }

    void EditorLayer::OnDuplicateEntity()
    {
        if (m_SceneState != SceneState::Edit) return;

        Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
        if (selectedEntity)
        {
            Entity newEntity = m_EditorScene->DuplicateEntity(selectedEntity);
            m_SceneHierarchyPanel.SetSelectedEntity(newEntity);
        }
    }

    bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e)
    {
        if (e.GetMouseButton() == Mouse::ButtonLeft)
        {
            if (m_ViewportHovered && !ImGuizmo::IsOver() && !Input::IsKeyPressed(Key::LeftAlt))
                m_SceneHierarchyPanel.SetSelectedEntity(m_HoveredEntity);
        }
        return false;
    }

    void EditorLayer::OnOverlayRender()
    {
        Renderer2D::BeginScene(m_EditorCamera);

        // Draw selected entity outline 
        if (Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity())
        {
            const TransformComponent& transform = selectedEntity.GetComponent<TransformComponent>();
            Renderer2D::DrawRect(transform.GetTransform(), glm::vec4(1.0f, 0.5f, 0.0f, 1.0f));
        }

        Renderer2D::EndScene();
    }
}