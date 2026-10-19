#pragma once
#include "Serialization/Json.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform.hpp>

namespace nu
{
    struct Transform
    {
        glm::vec3 position{ 0.0f };
        glm::vec3 rotation{ 0.0f }; // degrees
        glm::vec3 scale{ 1.0f };

        glm::mat4 matrix;

        const glm::mat4 GetMatrix()
        {
            auto mxt = glm::translate(position);
            auto mxr = glm::eulerAngleYXZ(glm::radians(rotation.y), glm::radians(rotation.x), glm::radians(rotation.z));
            auto mxs = glm::scale(scale);

            matrix = mxt * mxr * mxs;
            return matrix;
        }

        void Read(const json::value_t& value)
        {
            //JSON_READ(value, position);
            //JSON_READ(value, rotation);
            //JSON_READ(value, scale);
        }
    };
}