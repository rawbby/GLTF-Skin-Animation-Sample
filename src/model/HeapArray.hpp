#pragma once

#include <memory>
#include <cstdlib>

#include <span>

namespace model
{
    template <typename T>
    class HeapArray
    {
    public:

        using value_type = T;
        using reference = T &;
        using pointer = T *;
        using iterator = T *;

        using const_value_type = const T;
        using const_reference = const T &;
        using const_pointer = const T *;
        using const_iterator = const T *;

        using size_type = size_t;

    private:

        std::unique_ptr<value_type[]> m_data{};
        size_type m_size = 0;

    public:

        explicit HeapArray (size_type size)
                : m_data(std::make_unique<value_type[]>(size))
                , m_size(size)
        {
        }

        explicit HeapArray (std::span<const_value_type> data)
                : m_data(std::make_unique<value_type[]>(data.size()))
                , m_size(static_cast<size_type> (data.size()))
        {
            memcpy(m_data.get(), data.data(), data.size() * sizeof(value_type));
        }

        HeapArray (std::initializer_list<const_value_type> data)
                : m_data(std::make_unique<value_type[]>(data.size()))
                , m_size(static_cast<size_type> (data.size()))
        {
            memcpy(m_data.get(), data.begin(), data.size() * sizeof(value_type));
        }

        HeapArray (const HeapArray &other)
                : m_data(std::make_unique<value_type[]>(other.m_size))
                , m_size(other.m_size)
        {
            memcpy(m_data.get(), other.m_data, other.size() * sizeof(value_type));
        }

        HeapArray (HeapArray &&other) noexcept
                : m_data(nullptr)
                , m_size(other.m_size)
        {
            std::swap(m_data, other.m_data);
        }

        ~HeapArray () = default;

    public:

        HeapArray &operator= (const HeapArray &other)
        {
            m_data = std::make_unique<value_type[]>(other.m_size);
            m_size = other.m_size;
            memcpy(m_data.get(), other.m_data, other.size() * sizeof(value_type));
            return *this;
        }

        HeapArray &operator= (HeapArray &&other) noexcept
        {
            std::swap(m_data, other.m_data);
            m_size = other.m_size;
            return *this;
        }

        HeapArray &operator= (std::initializer_list<const_value_type> data)
        {
            memcpy(m_data.get(), data.begin(), data.size() * sizeof(value_type));
            return *this;
        }

    public:

        [[nodiscard]] iterator begin ()
        {
            return m_data.get();
        }

        [[nodiscard]] const_iterator begin () const
        {
            return m_data.get();
        }

        [[nodiscard]] iterator end ()
        {
            return &(m_data[m_size - 1]);
        }

        [[nodiscard]] const_iterator end () const
        {
            return &(m_data[m_size - 1]);
        }

        [[nodiscard]] const_iterator cbegin () const
        {
            return m_data.get();
        }

        [[nodiscard]] const_iterator cend () const
        {
            return &(m_data[m_size - 1]);
        }

    public:

        reference operator[] (size_type n)
        {
            return m_data[n];
        }

        const_reference operator[] (size_type n) const
        {
            return m_data[n];
        }

        [[nodiscard]] reference at (size_type n)
        {
            return m_data[n];
        }

        [[nodiscard]] const_reference at (size_type n) const
        {
            return m_data[n];
        }

        [[nodiscard]] pointer data () noexcept
        {
            return m_data.get();
        }

        [[nodiscard]] const_pointer data () const noexcept
        {
            return m_data.get();
        }

    public:

        [[nodiscard]] size_type size () const noexcept
        {
            return m_size;
        }
    };
}
