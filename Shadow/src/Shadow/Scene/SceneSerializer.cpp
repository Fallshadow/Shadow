#include "sdpch.h"
#include "SceneSerializer.h"
#include "Shadow/Project/Project.h"

#include <fstream>

#include <yaml-cpp/yaml.h>

namespace Shadow
{
    SceneSerializer::SceneSerializer(const Ref<Scene>& scene) : m_Scene(scene) { }

    void SceneSerializer::Serialize(const std::string& filepath)
    {
        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "Scene" << YAML::Value << "Untitled";
        out << YAML::EndMap;

        std::ofstream fout(filepath);
        fout << out.c_str();
    }

    bool SceneSerializer::Deserialize(const std::string& filepath)
    {
        YAML::Node data;
        try
        {
            data = YAML::LoadFile(filepath);
        }
        catch (YAML::ParserException e)
        {
            SD_CORE_ERROR("Failed to load .hazel file '{0}'\n     {1}", filepath, e.what());
            return false;
        }

        if (!data["Scene"])
            return false;

        std::string sceneName = data["Scene"].as<std::string>();
        SD_CORE_TRACE("Deserializing scene '{0}'", sceneName);

        return true;
    }
}