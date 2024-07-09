#include "sdpch.h"
#include "UUID.h"

#include <random>

#include <unordered_map>

namespace Shadow
{
    // 使用 std::random_device 可以产生比传统的伪随机数生成器更具随机性的种子，因为它利用了系统的真随机性源，如硬件噪声或操作系统提供的随机数设备。
    static std::random_device s_RandomDevice;
    // 使用 s_RandomDevice() 来获取一个随机数作为种子传递给 std::mt19937 随机数生成器。通过这种方式，可以保证种子的高质量随机性，从而生成更具真实随机性的随机数序列。
    static std::mt19937_64 s_Engine(s_RandomDevice());
    // 用于生成指定范围内均匀分布的整数随机数。它通常与随机数引擎（如 std::mt19937）配合使用，用于定义生成随机整数的范围。这里不限制范围，那就是指定类型范围的随机生成。
    static std::uniform_int_distribution<uint64_t> s_UniformDistribution;

    UUID::UUID() : m_UUID(s_UniformDistribution(s_Engine)) { }

    UUID::UUID(uint64_t uuid) : m_UUID(uuid) { }
}