#pragma once

#include <string>

namespace Shadow
{
    // 事件系统现在是立即阻塞类型。意味着当一个事件发生时，就会阻塞系统，进行立即处理，直到处理完，再返回。
    // 未来，更好的策略是将事件缓存到事件池，然后在Update的“事件”那一趴去集中处理。

    // 事件
    enum class EventType
    {
        None = 0,
        WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
        AppTick, AppUpdate, AppRender,
        KeyPressed, KeyReleased, KeyTyped,
        MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
    };

    // 事件分类 按位
    enum EventCategory
    {
        None = 0,
        EventCategoryApplication = BIT(0),
        EventCategoryInput = BIT(1),
        EventCategoryKeyboard = BIT(2),
        EventCategoryMouse = BIT(3),
        EventCategoryMouseButton = BIT(4),
    };

// 利用宏定义 定义事件类型相关函数和事件类型数据
// 获取静态事件类型、获取事件枚举类型（可重写）、获取事件名称（可重写）
// 静态事件类型为了在分发事件比较时拿正在发生的事件和无示例的类型T的事件类型作比较
#define EVENT_CLASS_TYPE(type) static EventType GetStaticType(){ return EventType::type; }\
virtual EventType GetEventType() const override { return GetStaticType();}\
virtual const char* GetName() const override {return #type;}

// 利用宏定义 定义事件分类类型函数
#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }

    // 事件基类
    class Event
    {
    public:
        virtual ~Event() = default;

        bool Handled = false;

        virtual EventType GetEventType() const = 0;
        virtual const char* GetName() const = 0;
        virtual int GetCategoryFlags() const = 0;
        // 只用于调试，所以不太关注性能（是不是应该用宏包起来）
        virtual std::string ToString() const { return GetName(); }

        bool IsInCategory(EventCategory category)
        {
            return GetCategoryFlags() & category;
        }
    protected:

    };

    class EventDispatcher
    {
    public:
        EventDispatcher(Event& event) : m_Event(event) { }

        template<typename T, typename F>
        bool Dispatch(const F& func)
        {
            if (m_Event.GetEventType() == T::GetStaticType())
            {
                m_Event.Handled |= func(static_cast<T&>(m_Event));
                return true;
            }

            return false;
        }
    private:
        Event& m_Event;
    };

    // 输出重载Event
    inline std::ostream& operator<<(std::ostream& os, const Event& e)
    {
        return os << e.ToString();
    }
}
