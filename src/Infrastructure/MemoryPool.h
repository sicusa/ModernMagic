#pragma once

#ifdef MM_ENABLE_MEMORYPOOL

#include <boost/pool/pool.hpp>
#include <boost/pool/object_pool.hpp>

MM_BEGIN

template <typename T>
class MemoryPool
{
public:
    static MemoryPool &GetInstance()
    {
        static MemoryPool<T> pool;
        return pool;
    }
    
    T* Malloc() { return _pool.malloc(); }
    T* Construct() { return _pool.construct(); }
    
    template <typename ...Args>
    T* Construct(Args&&... args) {
        return _pool.construct(std::forward<Args&&...>(args)...);
    }
    
    void Delete(T *ptr) { _pool.destroy(ptr); }
    
protected:
    MemoryPool()
    {
    }
    
private:
    boost::object_pool<T> _pool;
};

#else

MM_BEGIN

template <typename T>
class MemoryPool
{
public:
    static MemoryPool &GetInstance()
    {
        static MemoryPool<T> pool;
        return pool;
    }
    
    T *Malloc() {
		if (_allowedObj.empty()) {
			return static_cast<T*>(malloc(sizeof(T)));
		}
		else {
			T *ptr = *(_allowedObj.end() - 1);
			_allowedObj.pop_back();
			return ptr;
		}
	}
    T *Construct() {
		T *ptr = this->Malloc();
		try {
			new (ptr) T();
		}
		catch (...) {
			if (ptr) {
				this->Delete(ptr);
			}
			throw;
		}
		return ptr;
	}

	T *Construct(const T &other) {
		T *ptr = this->Malloc();
		try {
			new (ptr) T(other);
		}
		catch (...) {
			if (ptr) {
				this->Delete(ptr);
			}
			throw;
		}
		return ptr;
	}
    
    void Delete(T *ptr) {
		ptr->T::~T();
		_allowedObj.push_back(ptr);
	}
    
protected:
    MemoryPool()
    {
    }

private:
    std::vector<T*> _allowedObj;
};

#endif

MM_END