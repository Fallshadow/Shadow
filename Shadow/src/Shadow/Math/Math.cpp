#include "sdpch.h"
#include "Math.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

namespace Shadow::Math
{
    // 将一个仿射变换矩阵拆分成平移、旋转和缩放三个部分
    bool DecomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale)
    {
        // From glm::decompose in matrix_decompose.inl
        using namespace glm;
        using T = float;

        mat4 LocalMatrix(transform);

        // 矩阵归一化
        if (epsilonEqual(LocalMatrix[3][3], static_cast<float>(0), epsilon<T>()))
            return false;

        // 透视隔离
        // 检查矩阵中是否存在透视分量，如果存在，将其清零。
        // 检查矩阵最后一列的前三个元素，如果其不满足接近于0的条件，就是具有透视分量，判断成立，执行清零。
        if (epsilonNotEqual(LocalMatrix[0][3], static_cast<T>(0), epsilon<T>()) ||
            epsilonNotEqual(LocalMatrix[1][3], static_cast<T>(0), epsilon<T>()) ||
            epsilonNotEqual(LocalMatrix[2][3], static_cast<T>(0), epsilon<T>()))
        {
            // 清空透视分量
            LocalMatrix[0][3] = LocalMatrix[1][3] = LocalMatrix[2][3] = static_cast<T>(0);
            LocalMatrix[3][3] = static_cast<T>(1);
        }

        // 提取平移部分，将第四行前三个元素清零
        translation = vec3(LocalMatrix[3]);
        LocalMatrix[3] = vec4(0, 0, 0, LocalMatrix[3].w);

        vec3 Row[3];

        for (length_t i = 0; i < 3; ++i)
            for (length_t j = 0; j < 3; ++j)
                Row[i][j] = LocalMatrix[i][j];

        scale.x = length(Row[0]);
        Row[0] = detail::scale(Row[0], static_cast<T>(1));
        scale.y = length(Row[1]);
        Row[1] = detail::scale(Row[1], static_cast<T>(1));
        scale.z = length(Row[2]);
        Row[2] = detail::scale(Row[2], static_cast<T>(1));

        // 此时，矩阵（在 rows[] 中）是正交归一的。
        // 检查坐标系翻转。如果行列式为 - 1，则对矩阵和缩放因子取负值。
#if 0
        vec3 Pdum3 = cross(Row[1], Row[2]); // v3Cross(row[1], row[2], Pdum3);
        if (dot(Row[0], Pdum3) < 0)
        {
            for (length_t i = 0; i < 3; i++)
            {
                scale[i] *= static_cast<T>(-1);
                Row[i] *= static_cast<T>(-1);
            }
        }
#endif
        // 计算旋转
        rotation.y = asin(-Row[0][2]);
        if (cos(rotation.y) != 0) 
        {
            rotation.x = atan2(Row[1][2], Row[2][2]);
            rotation.z = atan2(Row[0][1], Row[0][0]);
        }
        else 
        {
            rotation.x = atan2(-Row[2][0], Row[1][1]);
            rotation.z = 0;
        }

        return true;
    }
}
