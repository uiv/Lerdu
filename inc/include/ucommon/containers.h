// Copyright (C) 2006-2010 David Sugar, Tycho Softworks.
//
// This file is part of GNU uCommon C++.
//
// GNU uCommon C++ is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// GNU uCommon C++ is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with GNU uCommon C++.  If not, see <http://www.gnu.org/licenses/>.

/**
 * Threadsafe object containers.  This is used to better define
 * object containers and manipulating classes which can be presumed to be
 * fully threadsafe and thread-aware.  This has to be defined separately
 * to assure correct order of preceeding headers as well as to better
 * organize the library for clarity.  Most of these classes and templates
 * work with classes derived from Object and LinkedObject and make use of
 * conditional for time constrained acquisition of managed objects.
 * @file ucommon/containers.h
 */

#ifndef _UCOMMON_CONTAINERS_H_
#define _UCOMMON_CONTAINERS_H_

#ifndef _UCOMMON_CONFIG_H_
#include <ucommon/platform.h>
#endif

#ifndef _UCOMMON_PROTOCOLS_H_
#include <ucommon/protocols.h>
#endif

#ifndef  _UCOMMON_LINKED_H_
#include <ucommon/linked.h>
#endif

#ifndef  _UCOMMON_MEMORY_H_
#include <ucommon/memory.h>
#endif

#ifndef  _UCOMMON_THREAD_H_
#include <ucommon/thread.h>
#endif

NAMESPACE_UCOMMON

/**
 * Linked allocator helper for linked_allocator template.  This is used
 * to alloc an array of typed objects tied to a free list in a single
 * operation.
 * @author David Sugar <dyfet@gnutelephony.org>
 */
class __EXPORT LinkedAllocator : private Conditional
{
protected:
    LinkedObject *freelist;

    LinkedAllocator();

    LinkedObject *get(void);

    LinkedObject *get(timeout_t timeout);

    void release(LinkedObject *node);

public:
    /**
     * Test if there is still objects in the free list.
     * @return true if there are objects.
     */
    operator bool();

    /**
     * Test if the free list is empty.
     * @return true if the free list is empty.
     */
    bool operator!();
};

/**
 * A thread-safe buffer for serializing and streaming class data.  While
 * the queue and stack operate by managing lists of reference pointers to
 * objects of various mixed kind, the buffer holds physical copies of objects
 * that being passed through it, and all must be the same size.  The buffer
 * class can be used stand-alone or with the typed bufferof template.  The
 * buffer is accessed in fifo order.
 * @author David Sugar <dyfet@gnutelephony.org>
 */
class __EXPORT Buffer : protected Conditional
{
private:
    size_t size, objsize;
    caddr_t buf, head, tail;
    unsigned count, limit;

public:
    /**
     * Create a buffer to hold a series of objects.
     * @param size of each object in buffer.
     * @param count of objects in the buffer.
     */
    Buffer(size_t size, size_t count);

    /**
     * Deallocate buffer and unblock any waiting threads.
     */
    virtual ~Buffer();

    /**
     * Get the size of the buffer.
     * @return size of the buffer.
     */
    unsigned getSize(void);

    /**
     * Get the number of objects in the buffer currently.
     * @return number of objects buffered.
     */
    unsigned getCount(void);

    /**
     * Get the next object from the buffer.
     * @param timeout to wait when buffer is empty in milliseconds.
     * @return pointer to next object in the buffer or NULL if timed out.
     */
    void *get(timeout_t timeout);

    /**
     * Get the next object from the buffer.  This blocks until an object
     * becomes available.
     * @return pointer to next object from buffer.
     */
    void *get(void);

    /**
     * Put (copy) an object into the buffer.  This blocks while the buffer
     * is full.
     * @param data to copy into the buffer.
     */
    void put(void *data);

    /**
     * Put (copy) an object into the buffer.
     * @param data to copy into the buffer.
     * @param timeout to wait if buffer is full.
     * @return true if copied, false if timed out while full.
     */
    bool put(void *data, timeout_t timeout);

    /**
     * Release must be called when we get an object from the buffer.  This
     * is because the pointer we return is a physical pointer to memory
     * that is part of the buffer.  The object we get cannot be removed or
     * the memory modified while the object is being used.
     */
    void release(void);

    /**
     * Copy the next object from the buffer.  This blocks until an object
     * becomes available.  Buffer is auto-released.
     * @param data pointer to copy into.
     */
    void copy(void *data);

    /**
     * Copy the next object from the buffer.  Buffer is auto-released.
     * @param data pointer to copy into.
     * @param timeout to wait when buffer is empty in milliseconds.
     * @return true if object copied, or false if timed out.
     */
    bool copy(void *data, timeout_t timeout);

    /**
     * Peek at pending data in buffer.  This returns a pointer to
     * objects relative to the head.  In effect it is the same as
     * get() for item = 0.
     * @param item to examine in buffer.
     * @return pointer to item or NULL if invalid item number.
     */
    void *peek(unsigned item);

    /**
     * Test if there is data waiting in the buffer.
     * @return true if buffer has data.
     */
    operator bool();

    /**
     * Test if the buffer is empty.
     * @return true if the buffer is empty.
     */
    bool operator!();
};

/**
 * Manage a thread-safe queue of objects through reference pointers.  This
 * can be particularly interesting when used to enqueue/dequeue reference
 * counted managed objects.  Thread-safe access is managed through a
 * conditional.  Both lifo and fifo forms of queue access  may be used.  A
 * pool of self-managed member objects are used to operate the queue.  This
 * queue is optimized for fifo access; while lifo is supported, it will be
 * slow.  If you need primarily lifo, you should use stack instead.
 * @author David Sugar <dyfet@gnutelephony.org>
 */
class __EXPORT queue : protected OrderedIndex, protected Conditional
{
private:
    mempager *pager;
    LinkedObject *freelist;
    size_t used;

    class __LOCAL member : public OrderedObject
    {
    public:
        member(queue *q, ObjectProtocol *obj);
        ObjectProtocol *object;
    };

    friend class member;

protected:
    size_t limit;

public:
    /**
     * Create a queue that uses a memory pager for internally managed
     * member objects for a specified maximum number of object pointers.
     * @param pager to use for internal member object or NULL to use heap.
     * @param number of pointers that can be in the queue or 0 for unlimited.
     * size limit.
     */
    queue(mempager *pager = NULL, size_t number = 0);

    /**
     * Destroy queue.  If no mempager is used, then frees heap.
     */
    ~queue();

    /**
     * Remove a specific object pointer for the queue.  This can remove
     * a member from any location in the queue, whether beginning, end, or
     * somewhere in the middle.  This also releases the object.
     * @param object to remove.
     * @return true if object was removed, false if not found.
     */
    bool remove(ObjectProtocol *object);

    /**
     * Post an object into the queue by it's pointer.  This can wait for
     * a specified timeout if the queue is full, for example, for another
     * thread to remove an object pointer.  This also retains the object.
     * @param object to post.
     * @param timeout to wait if queue is full in milliseconds.
     * @return true if object posted, false if queue full and timeout expired.
     */
    bool post(ObjectProtocol *object, timeout_t timeout = 0);

    /**
     * Examine pending existing object in queue.  Does not remove it.
     * @param number of elements back.
     * @return object in queue or NULL if invalid value.
     */
    ObjectProtocol *peek(unsigned offset = 0);

    /**
     * Get and remove last object posted to the queue.  This can wait for
     * a specified timeout of the queue is empty.  The object is still
     * retained and must be released or deleted by the receiving function.
     * @param timeout to wait if empty in milliseconds.
     * @return object from queue or NULL if empty and timed out.
     */
    ObjectProtocol *fifo(timeout_t timeout = 0);

    /**
     * Get and remove first object posted to the queue.  This can wait for
     * a specified timeout of the queue is empty.  The object is still
     * retained and must be released or deleted by the receiving function.
     * @param timeout to wait if empty in milliseconds.
     * @return object from queue or NULL if empty and timed out.
     */
    ObjectProtocol *lifo(timeout_t timeout = 0);

    /**
     * Get number of object points currently in the queue.
     * @return number of objects in queue.
     */
    size_t getCount(void);

    /**
     * Convenience function to remove an object from the queue.
     * @param queue to remove object from.
     * @param object to remove.
     * @return true if removed, false if not found.
     */
    static bool remove(queue& queue, ObjectProtocol *object)
        {return queue.remove(object);};

    /**
     * Convenience function to post object into the queue.
     * @param queue to post into.
     * @param object to post.
     * @param timeout to wait if full.
     * @return true if posted, false if timed out while full.
     */
    static bool post(queue& queue, ObjectProtocol *object, timeout_t timeout = 0)
        {return queue.post(object, timeout);};

    /**
     * Convenience function get first object from the queue.
     * @param queue to get from.
     * @param timeout to wait if empty.
     * @return first object or NULL if timed out empty.
     */
    static ObjectProtocol *fifo(queue& queue, timeout_t timeout = 0)
        {return queue.fifo(timeout);};

    /**
     * Convenience function get last object from the queue.
     * @param queue to get from.
     * @param timeout to wait if empty.
     * @return last object or NULL if timed out empty.
     */
    static ObjectProtocol *lifo(queue& queue, timeout_t timeout = 0)
        {return queue.lifo(timeout);};

    /**
     * Convenience function to get count of objects in the queue.
     * @param queue to count.
     * @return number of objects in the queue.
     */
    static size_t count(queue& queue)
        {return queue.getCount();};

    inline ObjectProtocol *operator[](unsigned pos)
        {return peek(pos);};
};

/**
 * Manage a thread-safe stack of objects through reference pointers.  This
 * Thread-safe access is managed through a conditional.  This differs from
 * the queue in lifo mode because delinking the last object is immediate,
 * and because it has much less overhead.  A pool of self-managed
 * member objects are used to operate the stack.
 * @author David Sugar <dyfet@gnutelephony.org>
 */
class __EXPORT stack : protected Conditional
{
private:
    LinkedObject *freelist, *usedlist;
    mempager *pager;
    size_t used;

    class __LOCAL member : public LinkedObject
    {
    public:
        member(stack *s, ObjectProtocol *obj);
        ObjectProtocol *object;
    };

    friend class member;

protected:
    size_t limit;

public:
    /**
     * Create a stack that uses a memory pager for internally managed
     * member objects for a specified maximum number of object pointers.
     * @param pager to use for internal member object or NULL to use heap.
     * @param number of pointers that can be in the stack or 0 if unlimited.
     */
    stack(mempager *pager = NULL, size_t number = 0);

    /**
     * Destroy queue.  If no pager is used, then frees heap.
     */
    ~stack();

    /**
     * Remove a specific object pointer for the queue.  This can remove
     * a member from any location in the queue, whether beginning, end, or
     * somewhere in the middle.  This also releases the object.
     * @param object to remove.
     * @return true if object was removed, false if not found.
     */
    bool remove(ObjectProtocol *object);

    /**
     * Push an object into the stack by it's pointer.  This can wait for
     * a specified timeout if the stack is full, for example, for another
     * thread to remove an object pointer.  This also retains the object.
     * @param object to push.
     * @param timeout to wait if stack is full in milliseconds.
     * @return true if object pushed, false if stack full and timeout expired.
     */
    bool push(ObjectProtocol *object, timeout_t timeout = 0);

    /**
     * Get and remove last object pushed on the stack.  This can wait for
     * a specified timeout of the stack is empty.  The object is still
     * retained and must be released or deleted by the receiving function.
     * @param timeout to wait if empty in milliseconds.
     * @return object pulled from stack or NULL if empty and timed out.
     */
    ObjectProtocol *pull(timeout_t timeout = 0);

    /**
     * Examine an existing object on the stack.
     * @param offset to stack entry.
     * @return object examined.
     */
    ObjectProtocol *peek(unsigned offset = 0);

    /**
     * Get number of object points currently in the stack.
     * @return number of objects in stack.
     */
    size_t getCount(void);

    /**
     * Convenience function to remove an object from the stacl.
     * @param stack to remove object from.
     * @param object to remove.
     * @return true if removed, false if not found.
     */
    static inline bool remove(stack& stack, ObjectProtocol *object)
        {return stack.remove(object);};

    /**
     * Convenience function to push object into the stack.
     * @param stack to push into.
     * @param object to push.
     * @param timeout to wait if full.
     * @return true if pusheded, false if timed out while full.
     */
    static inline bool push(stack& stack, ObjectProtocol *object, timeout_t timeout = 0)
        {return stack.push(object, timeout);};

    /**
     * Convenience function pull last object from the stack.
     * @param stack to get from.
     * @param timeout to wait if empty.
     * @return last object or NULL if timed out empty.
     */
    static inline ObjectProtocol *pull(stack& stack, timeout_t timeout = 0)
        {return stack.pull(timeout);};

    /**
     * Convenience function to get count of objects in the stack.
     * @param stack to count.
     * @return number of objects in the stack.
     */
    static inline size_t count(stack& stack)
        {return stack.getCount();};

    inline ObjectProtocol *operator[](unsigned pos)
        {return peek(pos);};
};

/**
 * Linked allocator template to gather linked objects.  This allocates the
 * object pool in a single array as a single heap allocation, and releases
 * the whole pool with a single delete when done.  It is also threadsafe.
 * The types used must be derived of LinkedObject.
 * @author David Sugar <dyfet@gnutelephony.org>
 */
template <class T>
class linked_allocator : public LinkedAllocator
{
private:
    T* array;

public:
    inline linked_allocator(size_t size) : LinkedAllocator() {
        array = new T[size];
        for(unsigned i = 0; i < size; ++i)
            array[i].enlist(&freelist);
    }

    ~linked_allocator()
        {delete[] array;};

    inline T *get(void)
        {return static_cast<T *>(LinkedAllocator::get());};

    inline T *get(timeout_t timeout)
        {return static_cast<T *>(LinkedAllocator::get(timeout));};

    inline void release(T *node)
        {LinkedAllocator::release(node);};
};

/**
 * A templated typed class for buffering of objects.  This operates as a
 * fifo buffer of typed objects which are physically copied into the buffer.
 * The objects that are buffered are accessed from allocated buffer space.
 * As designed this may be used with multiple producer threads and one
 * consumer thread.  To use multiple consumers, one can copy the typed object
 * from the buffer through the get pointer and then call release.  The
 * copied object can then be used safely.  This is what the copy method is
 * used for.
 * @author David Sugar <dyfet@gnutelephony.org>
 */
template<class T>
class bufferof : public Buffer
{
public:
    /**
     * Create a buffer to hold a series of typed objects.
     * @param count of typed objects in the buffer.
     */
    inline bufferof(unsigned count) :
        Buffer(sizeof(T), count) {};

    /**
     * Get the next typed object from the buffer.  This blocks until an object
     * becomes available.
     * @return pointer to next typed object from buffer.
     */
    inline T *get(void)
        {return static_cast<T*>(get());};

    /**
     * Get the next typed object from the buffer.
     * @param timeout to wait when buffer is empty in milliseconds.
     * @return pointer to next typed object in the buffer or NULL if timed out.
     */
    inline T *get(timeout_t timeout)
        {return static_cast<T*>(get(timeout));};

    /**
     * Put (copy) a typed object into the buffer.  This blocks while the buffer
     * is full.
     * @param object to copy into the buffer.
     */
    inline void put(T *object)
        {put(object);};

    /**
     * Put (copy) an object into the buffer.
     * @param object to copy into the buffer.
     * @param timeout to wait if buffer is full.
     * @return true if copied, false if timed out while full.
     */
    inline bool put(T *object, timeout_t timeout)
        {return put(object, timeout);};

    /**
     * Copy the next typed object from the buffer.  This blocks until an object
     * becomes available.
     * @param object pointer to copy typed object into.
     */
    inline void copy(T *object)
        {copy(object);};

    /**
     * Copy the next typed object from the buffer.
     * @param object pointer to copy typed object into.
     * @param timeout to wait when buffer is empty in milliseconds.
     * @return true if object copied, or false if timed out.
     */
    inline bool get(T *object, timeout_t timeout)
        {return copy(object, timeout);};

    /**
     * Examine past item in the buffer.  This is a typecast of the peek
     * operation.
     * @param item in buffer.
     * @return item pointer if valid or NULL.
     */
    inline T *peek(unsigned item)
        {return static_cast<T *>(Buffer::peek(item));};

    /**
     * Examine past item in the buffer.  This is a typecast of the peek
     * operation.
     * @param item in buffer.
     * @return item pointer if valid or NULL.
     */
    inline T *operator[](unsigned item)
        {return static_cast<T *>(queue::peek(item));};

};

/**
 * A templated typed class for thread-safe stack of object pointers.  This
 * allows one to use the stack class in a typesafe manner for a specific
 * object type derived from Object rather than generically for any derived
 * object class.
 * @author David Sugar <dyfet@gnutelephony.org>
 */
template<class T>
class stackof : public stack
{
public:
    /**
     * Create templated stack of typed objects.
     * @param memory pool for internal use of stack.
     * @param size of stack to construct.  Uses 0 if no size limit.
     */
    inline stackof(mempager *memory, size_t size = 0) : stack(memory, size) {};

    /**
     * Remove a specific typed object pointer for the stack.  This can remove
     * a member from any location in the stack, whether beginning, end, or
     * somewhere in the middle.  This releases the object.
     * @param object to remove.
     * @return true if object was removed, false if not found.
     */
    inline bool remove(T *object)
        {return stack::remove(object);};

    /**
     * Push a typed object into the stack by it's pointer.  This can wait for
     * a specified timeout if the queue is full, for example, for another
     * thread to remove an object pointer.  This retains the object.
     * @param object to push.
     * @param timeout to wait if queue is full in milliseconds.
     * @return true if object pushed, false if queue full and timeout expired.
     */
    inline bool push(T *object, timeout_t timeout = 0)
        {return stack::push(object);};

    /**
     * Get and remove last typed object posted to the stack.  This can wait for
     * a specified timeout of the stack is empty.  The object is still retained
     * and must be released or deleted by the receiving function.
     * @param timeout to wait if empty in milliseconds.
     * @return object from queue or NULL if empty and timed out.
     */
    inline T *pull(timeout_t timeout = 0)
        {return static_cast<T *>(stack::pull(timeout));};

    /**
     * Examine past item in the stack.  This is a typecast of the peek
     * operation.
     * @param offset in stack.
     * @return item pointer if valid or NULL.
     */
    inline T *peek(unsigned offset = 0)
        {return static_cast<T *>(stack::peek(offset));};

    /**
     * Examine past item in the stack.  This is a typecast of the peek
     * operation.
     * @param offset in stack.
     * @return item pointer if valid or NULL.
     */
    inline T *operator[](unsigned offset)
        {return static_cast<T *>(stack::peek(offset));};

};

/**
 * A templated typed class for thread-safe queue of object pointers.  This
 * allows one to use the queue class in a typesafe manner for a specific
 * object type derived from Object rather than generically for any derived
 * object class.
 * @author David Sugar <dyfet@gnutelephony.org>
 */
template<class T>
class queueof : public queue
{
public:
    /**
     * Create templated queue of typed objects.
     * @param memory pool for internal use by queue.
     * @param size of queue to construct.  Uses 0 if no size limit.
     */
    inline queueof(mempager *memory, size_t size = 0) : queue(memory, size) {};

    /**
     * Remove a specific typed object pointer for the queue.  This can remove
     * a member from any location in the queue, whether beginning, end, or
     * somewhere in the middle. This releases the object.
     * @param object to remove.
     * @return true if object was removed, false if not found.
     */
    inline bool remove(T *object)
        {return queue::remove(object);};

    /**
     * Post a typed object into the queue by it's pointer.  This can wait for
     * a specified timeout if the queue is full, for example, for another
     * thread to remove an object pointer.  This retains the object.
     * @param object to post.
     * @param timeout to wait if queue is full in milliseconds.
     * @return true if object posted, false if queue full and timeout expired.
     */
    inline bool post(T *object, timeout_t timeout = 0)
        {return queue::post(object);};

    /**
     * Get and remove first typed object posted to the queue.  This can wait for
     * a specified timeut of the queue is empty.  The object is still retained
     * and must be released or deleted by the receiving function.
     * @param timeout to wait if empty in milliseconds.
     * @return object from queue or NULL if empty and timed out.
     */
    inline T *fifo(timeout_t timeout = 0)
        {return static_cast<T *>(queue::fifo(timeout));};

    /**
     * Get and remove last typed object posted to the queue.  This can wait for
     * a specified timeout of the queue is empty.  The object is still retained
     * and must be released or deleted by the receiving function.
     * @param timeout to wait if empty in milliseconds.
     * @return object from queue or NULL if empty and timed out.
     */
    inline T *lifo(timeout_t timeout = 0)
        {return static_cast<T *>(queue::lifo(timeout));};

    /**
     * Examine past item in the queue.  This is a typecast of the peek
     * operation.
     * @param offset in queue.
     * @return item pointer if valid or NULL.
     */
    inline T *peek(unsigned offset = 0)
        {return static_cast<T *>(queue::peek(offset));};

    /**
     * Examine past item in the queue.  This is a typecast of the peek
     * operation.
     * @param offset in queue.
     * @return item pointer if valid or NULL.
     */
    inline T *operator[](unsigned offset)
        {return static_cast<T *>(queue::peek(offset));};

};

/**
 * Convenience type for using thread-safe object stacks.
 */
typedef stack stack_t;

/**
 * Convenience type for using thread-safe object fifo (queue).
 */
typedef queue fifo_t;

/**
 * Convenience function to push an object onto a stack.
 * @param stack to push into.
 * @param object to push.
 */
inline void push(stack_t &stack, ObjectProtocol *object)
    {stack.push(object);}

/**
 * Convenience function to pull an object from a stack.
 * @param stack to pull from.
 * @param timeout to wait to pull.
 * @return object pulled.
 */
inline ObjectProtocol *pull(stack_t &stack, timeout_t timeout = Timer::inf)
    {return stack.pull(timeout);}

/**
 * Convenience function to remove an object from a stack.
 * @param stack to remove from.
 * @param object to remove.
 */
inline void remove(stack_t &stack, ObjectProtocol *object)
    {stack.remove(object);}

/**
 * Convenience function to push an object onto a fifo.
 * @param fifo to push into.
 * @param object to push.
 */
inline void push(fifo_t &fifo, ObjectProtocol *object)
    {fifo.post(object);}

/**
 * Convenience function to pull an object from a fifo.
 * @param fifo to pull from.
 * @param timeout to wait to pull.
 * @return object pulled.
 */
inline ObjectProtocol *pull(fifo_t &fifo, timeout_t timeout = Timer::inf)
    {return fifo.fifo(timeout);}

/**
 * Convenience function to remove an object from a fifo.
 * @param fifo to remove from.
 * @param object to remove.
 */
inline void remove(fifo_t &fifo, ObjectProtocol *object)
    {fifo.remove(object);}

END_NAMESPACE

#endif
