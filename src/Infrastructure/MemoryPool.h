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
    
    T* Malloc() { return static_cast<T*>(malloc(sizeof(T))); }
    T* Construct() { return new T(); }
    
    template <typename Arg>
    T* Construct(Arg &&arg) {
        return new T(std::forward<Arg>(arg));
    }
    
    void Delete(T *ptr) { delete ptr; }
    
protected:
    MemoryPool()
    {
    }

private:
    //boost::object_pool<T> _pool;
};

#endif

MM_END