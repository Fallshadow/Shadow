#pragma once

#include "Shadow/Core/Tool/KeyCodes.h"
#include "Shadow/Core/Tool/MouseCodes.h"

#include <glm/glm.hpp>

namespace Shadow
{
    class Input
    {
    public:
        static bool IsKeyPressed(KeyCode key);
        static bool IsMouseButtonPressed(MouseCode button);
        static glm::vec2 GetMousePosition();
        static float GetMouseX();
        static float GetMouseY();
    };
}
