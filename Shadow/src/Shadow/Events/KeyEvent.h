#pragma once

#include "Shadow/Events/Event.h"
#include "Shadow/Core/Tool/KeyCodes.h"

#include <sstream>

namespace Shadow
{
    class KeyEvent : public Event
    {
    public:
        KeyCode GetKeyCode() const { return m_KeyCode; }

        EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
    protected:
        // 按键基类不应该被实例化，所以保护起来
        KeyEvent(const KeyCode keycode) : m_KeyCode(keycode) { }
        KeyCode m_KeyCode;
    };

    class KeyPressedEvent : public KeyEvent
    {
    public:
        KeyPressedEvent(const KeyCode keycode, bool isRepeat = false) : KeyEvent(keycode), m_IsRepeat(isRepeat) {}
        bool IsRepeat() const { return m_IsRepeat; }

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "KeyPressedEvent: " << m_KeyCode << " (repeat = " << m_IsRepeat << ")";
            return ss.str();
        }
        EVENT_CLASS_TYPE(KeyPressed)
    private:
        bool m_IsRepeat;
    };

    class KeyReleasedEvent : public KeyEvent
    {
    public:
        KeyReleasedEvent(const KeyCode keycode) : KeyEvent(keycode) { }
        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "KeyReleasedEvent : " << m_KeyCode;
            return ss.str();
        }
        EVENT_CLASS_TYPE(KeyReleased)
    };

    class KeyTypedEvent : public KeyEvent
    {
    public:
        KeyTypedEvent(const KeyCode keycode) : KeyEvent(keycode) {}
        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "KeyTypedEvent : " << m_KeyCode;
            return ss.str();
        }
        EVENT_CLASS_TYPE(KeyTyped)
    };
}
