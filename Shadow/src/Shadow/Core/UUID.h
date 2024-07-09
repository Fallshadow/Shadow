#pragma once

namespace Shadow
{
    class UUID
    {
    public:
        UUID();
        UUID(uint64_t uuid);
        UUID(const UUID&) = default;

        operator uint64_t() const { return m_UUID; }
    private:
        uint64_t m_UUID;
    };
}

namespace std
{
    // 通过定义这样的哈希函数，可以让Shadow::UUID类型的对象在需要使用哈希函数的标准库容器中正常工作。
    // 例如 std::unordered_map 和 std::unordered_set 。在这些容器中，哈希函数用于将键值对或元素映射到容器内部的桶中，以支持高效的查找和插入操作。

    // 在这段代码中，使用了模板元编程的技术来实例化std::hash结构体的模板。
    // std::hash是一个用于计算哈希值的模板类，其中的operator()函数用于执行哈希计算。
    // 对于特定类型Shadow::UUID，通过对std::hash进行特化，为该类型提供了自定义的哈希算法。这里的算法直接就是强转为uint64_t，因为本身就是随机数了，不会重复。
    template <typename T> struct hash;

    template<>
    struct hash<Shadow::UUID>
    {
        std::size_t operator()(const Shadow::UUID& uuid) const
        {
            return (uint64_t)uuid;
        }
    };
}