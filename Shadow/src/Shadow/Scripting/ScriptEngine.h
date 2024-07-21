#pragma once

#include "Shadow/Scene/Scene.h"
#include "Shadow/Scene/Entity.h"

#include <filesystem>
#include <string>
#include <map>

// Mono Runtime 是用 C# 编写的。
// 然而，Mono Runtime 提供了一组 C 语言的 API，这些 API 允许开发者使用 C 或者其他语言来与 Mono Runtime 进行交互。
// extern "C" 部分是为了确保 C++ 代码能够正确地调用这些 C 语言的 Mono Runtime API。
extern "C"
{
    typedef struct _MonoClass MonoClass;
    typedef struct _MonoObject MonoObject;
    typedef struct _MonoMethod MonoMethod;
    typedef struct _MonoAssembly MonoAssembly;
    typedef struct _MonoImage MonoImage;
    typedef struct _MonoClassField MonoClassField;
    typedef struct _MonoString MonoString;
}

namespace Shadow
{
    enum class ScriptFieldType
    {
        None = 0,
        Float, Double,
        Bool, Char, Byte, Short, Int, Long,
        UByte, UShort, UInt, ULong,
        Vector2, Vector3, Vector4,
        Entity
    };

    struct ScriptField
    {
        ScriptFieldType Type;
        std::string Name;

        MonoClassField* ClassField;
    };

    // ScriptField + data storage
    // ScriptFieldInstance 结构体可以方便地读取和存储不同类型的值，并且通过模板的方式实现了通用性和类型安全。
    struct ScriptFieldInstance
    {
        ScriptField Field;

        ScriptFieldInstance()
        {
            memset(m_Buffer, 0, sizeof(m_Buffer));
        }

        template<typename T>
        T GetValue()
        {
            static_assert(sizeof(T) <= 16, "Type too large!");
            return *(T*)m_Buffer;
        }

        template<typename T>
        void SetValue(T value)
        {
            static_assert(sizeof(T) <= 16, "Type too large!");
            memcpy(m_Buffer, &value, sizeof(T));
        }
    private:
        uint8_t m_Buffer[16];

        friend class ScriptEngine;
        friend class ScriptInstance;
    };

    using ScriptFieldMap = std::unordered_map<std::string, ScriptFieldInstance>;

    // 封装与 Mono 运行时系统的交互，提供了一些方法来实例化对象、获取和调用方法，以及管理脚本字段。
    class ScriptClass
    {
    public:
        ScriptClass() = default;
        ScriptClass(const std::string& classNamespace, const std::string& className, bool isCore = false);

        MonoObject* Instantiate();
        MonoMethod* GetMethod(const std::string& name, int parameterCount);
        MonoObject* InvokeMethod(MonoObject* instance, MonoMethod* method, void** params = nullptr);

        const std::map<std::string, ScriptField>& GetFields() const { return m_Fields; }
    private:
        MonoClass* m_MonoClass = nullptr;
        std::map<std::string, ScriptField> m_Fields;
        std::string m_ClassNamespace;
        std::string m_ClassName;

        friend class ScriptEngine;
    };

    class ScriptInstance
    {
    public:
        ScriptInstance(Ref<ScriptClass> scriptClass, Entity entity);

        void InvokeOnCreate();
        void InvokeOnUpdate(float ts);

        template<typename T>
        T GetFieldValue(const std::string& name)
        {
            static_assert(sizeof(T) <= 16, "Type too large!");

            bool success = GetFieldValueInternal(name, s_FieldValueBuffer);
            if (!success)
                return T();

            return *(T*)s_FieldValueBuffer;
        }

        template<typename T>
        void SetFieldValue(const std::string& name, T value)
        {
            static_assert(sizeof(T) <= 16, "Type too large!");
            SetFieldValueInternal(name, &value);
        }

        MonoObject* GetManagedObject() { return m_Instance; }
        Ref<ScriptClass> GetScriptClass() { return m_ScriptClass; }
    private:
        bool GetFieldValueInternal(const std::string& name, void* buffer);
        bool SetFieldValueInternal(const std::string& name, const void* value);
    private:
        Ref<ScriptClass> m_ScriptClass;
        // 存储mono获取的字段信息
        inline static char s_FieldValueBuffer[16];

        MonoObject* m_Instance = nullptr;
        MonoMethod* m_Constructor = nullptr;
        MonoMethod* m_OnCreateMethod = nullptr;
        MonoMethod* m_OnUpdateMethod = nullptr;

        friend class ScriptEngine;
        friend struct ScriptFieldInstance;
    };

    class ScriptEngine
    {
    public:
        static void Init();
        static void Shutdown();

        static bool LoadAssembly(const std::filesystem::path& filepath);
        static bool LoadAppAssembly(const std::filesystem::path& filepath);

        static bool EntityClassExists(const std::string& fullClassName);
        static void OnCreateEntity(Entity entity);
        static void OnUpdateEntity(Entity entity, TimeStep ts);
        static void OnRuntimeStart(Scene* scene);
        static void OnRuntimeStop();
        static void ReloadAssembly();

        static Scene* GetSceneContext();
        static Ref<ScriptClass> GetEntityClass(const std::string& name);
        static std::unordered_map<std::string, Ref<ScriptClass>> GetEntityClasses();
        static ScriptFieldMap& GetScriptFieldMap(Entity entity);
        static MonoImage* GetCoreAssemblyImage();
        static MonoObject* GetManagedInstance(UUID uuid);
        static Ref<ScriptInstance> GetEntityScriptInstance(UUID entityID);

        static MonoString* CreateString(const char* string);
    private:
        static void InitMono();
        static void ShutdownMono();

        static void LoadAssemblyClasses();
        static MonoObject* InstantiateClass(MonoClass* monoClass);

        friend class ScriptClass;
        friend class ScriptGlue;
    };

    namespace Utils
    {
        inline const char* ScriptFieldTypeToString(ScriptFieldType fieldType)
        {
            switch (fieldType)
            {
            case ScriptFieldType::None:    return "None";
            case ScriptFieldType::Float:   return "Float";
            case ScriptFieldType::Double:  return "Double";
            case ScriptFieldType::Bool:    return "Bool";
            case ScriptFieldType::Char:    return "Char";
            case ScriptFieldType::Byte:    return "Byte";
            case ScriptFieldType::Short:   return "Short";
            case ScriptFieldType::Int:     return "Int";
            case ScriptFieldType::Long:    return "Long";
            case ScriptFieldType::UByte:   return "UByte";
            case ScriptFieldType::UShort:  return "UShort";
            case ScriptFieldType::UInt:    return "UInt";
            case ScriptFieldType::ULong:   return "ULong";
            case ScriptFieldType::Vector2: return "Vector2";
            case ScriptFieldType::Vector3: return "Vector3";
            case ScriptFieldType::Vector4: return "Vector4";
            case ScriptFieldType::Entity:  return "Entity";
            }
            SD_CORE_ASSERT(false, "Unknown ScriptFieldType");
            return "None";
        }

        inline ScriptFieldType ScriptFieldTypeFromString(std::string_view fieldType)
        {
            if (fieldType == "None")    return ScriptFieldType::None;
            if (fieldType == "Float")   return ScriptFieldType::Float;
            if (fieldType == "Double")  return ScriptFieldType::Double;
            if (fieldType == "Bool")    return ScriptFieldType::Bool;
            if (fieldType == "Char")    return ScriptFieldType::Char;
            if (fieldType == "Byte")    return ScriptFieldType::Byte;
            if (fieldType == "Short")   return ScriptFieldType::Short;
            if (fieldType == "Int")     return ScriptFieldType::Int;
            if (fieldType == "Long")    return ScriptFieldType::Long;
            if (fieldType == "UByte")   return ScriptFieldType::UByte;
            if (fieldType == "UShort")  return ScriptFieldType::UShort;
            if (fieldType == "UInt")    return ScriptFieldType::UInt;
            if (fieldType == "ULong")   return ScriptFieldType::ULong;
            if (fieldType == "Vector2") return ScriptFieldType::Vector2;
            if (fieldType == "Vector3") return ScriptFieldType::Vector3;
            if (fieldType == "Vector4") return ScriptFieldType::Vector4;
            if (fieldType == "Entity")  return ScriptFieldType::Entity;

            SD_CORE_ASSERT(false, "Unknown ScriptFieldType");
            return ScriptFieldType::None;
        }

    }
}
