#pragma once

#include <cstddef>

#include <memory>

template <typename T>
class raii_ptr
{
public:
    typedef T* pointer;
    typedef T value_type;
    typedef T& reference;

private:
    pointer data;

public:
    raii_ptr()
		: data(NULL)
    {}

    explicit raii_ptr(pointer p)
        : data(p)
    {}

    ~raii_ptr()
    {
		delete data;
    }

    raii_ptr(const raii_ptr& other)
		: data(other.data)
    {
        const_cast<raii_ptr&>(other).data = NULL;
    }

    raii_ptr& operator=(const raii_ptr& other)
    {
		if (other.data != NULL)
		{
			data = new T;
			*data = *other.data;
		}
		else
		{
			delete data;
			data = other.data;
		}
		return *this;
    }

	raii_ptr& operator=(const pointer other)
	{
		if (other != NULL)
		{
			data = new T;
			*data = *other;
		}
		else
		{
			delete data;
			data = other;
		}
		return *this;
	}

    reference operator*() const
    {
        return *data;
    }

    pointer operator->() const
    {
        return data;
    }

	pointer get() const
	{
		return data;
	}

    operator bool() const
    {
        return data != NULL;
    }
};

template <typename T>
raii_ptr<T> make_default_ptr()
{
    return raii_ptr<T>(new T());
}
