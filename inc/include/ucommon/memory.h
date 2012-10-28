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
 * Private heaps, pools, and associations.
 * Private heaps often can reduce locking contention in threaded applications
 * since they do not require using the global "malloc" function.  Private
 * heaps also can be used as auto-release heaps, where all memory allocated
 * and handled out for small objects can be automatically released all at once.
 * Pager pools are used to optimize system allocation around page boundaries.
 * Associations allow private memory to be tagged and found by string
 * identifiers.
 * @file ucommon/memory.h
 */

#ifndef _UCOMMON_MEMORY_H_
#define _UCOMMON_MEMORY_H_

#ifndef _UCOMMON_CONFIG_H_
#include <ucommon/platform.h>
#endif

#ifndef _UCOMMON_PROTOCOLS_H_
#include <ucommon/protocols.h>
#endif

#ifndef  _UCOMMON_LINKED_H_
#include <ucommon/linked.h>
#endif

NAMESPACE_UCOMMON

class PagerPool;

/**
 * A memory protocol pager for private heap manager.  This is used to allocate
 * in an optimized manner, as it assumes no mutex locks are held or used as
 * part of it's own internal processing.  It also is designed for optimized
 * performance.
 * @author David Sugar <dyfet@gnutelephony.org>
 */
class __EXPORT memalloc : public MemoryProtocol, protected LockingProtocol
{
private:
    friend class bufpager;

    size_t pagesize, align;
    unsigned count;

    typedef struct mempage {
        struct mempage *next;
        union {
            void *memalign;
            unsigned used;
        };
    }   page_t;

    page_t *page;

protected:
    unsigned limit;

    /**
     * Acquire a new page from the heap.  This is mostly used internally.
     * @return page structure of the newly acquired memory page.
     */
    page_t *pager(void);

public:
    /**
     * Construct a memory pager.
     * @param page size to use or 0 for OS allocation size.
     */
    memalloc(size_t page = 0);

    /**
     * Destroy a memory pager.  Release all pages back to the heap at once.
     */
    virtual ~memalloc();

    /**
     * Get the number of pages that have been allocated from the real heap.
     * @return pages allocated from heap.
     */
    inline unsigned getPages(void)
        {return count;};

    /**
     * Get the maximum number of pages that are permitted.  One can use a
     * derived class to set and enforce a maximum limit to the number of
     * pages that will be allocated from the real heap.  This is often used
     * to detect and bring down apps that are leaking.
     * @return page allocation limit.
     */
    inline unsigned getLimit(void)
        {return limit;};

    /**
     * Get the size of a memory page.
     * @return size of each pager heap allocation.
     */
    inline unsigned getAlloc(void)
        {return pagesize;};

    /**
     * Determine fragmentation level of acquired heap pages.  This is
     * represented as an average % utilization (0-100) and represents the
     * used portion of each allocated heap page verse the page size.  Since
     * requests that cannot fit on an already allocated page are moved into
     * a new page, there is some unusable space left over at the end of the
     * page.  When utilization approaches 100, this is good.  A low utilization
     * may suggest a larger page size should be used.
     * @return pager utilization.
     */
    unsigned utilization(void);

    /**
     * Purge all allocated memory and heap pages immediately.
     */
    void purge(void);

    /**
     * Allocate memory from the pager heap.  The size of the request must be
     * less than the size of the memory page used.  This implements the
     * memory protocol allocation method.
     * @param size of memory request.
     * @return allocated memory or NULL if not possible.
     */
    virtual void *_alloc(size_t size);
};

/**
 * A managed private heap for small allocations.  This is used to allocate
 * a large number of small objects from a paged heap as needed and to then
 * release them together all at once.  This pattern has significantly less
 * overhead than using malloc and offers less locking contention since the
 * memory pager can also have it's own mutex.  Pager pool allocated memory
 * is always aligned to the optimal data size for the cpu bus and pages are
 * themselves created from memory aligned allocations.  A page size for a
 * memory pager should be some multiple of the OS paging size.
 *
 * The mempager uses a strategy of allocating fixed size pages as needed
 * from the real heap and allocating objects from these pages as needed.
 * A new page is allocated from the real heap when there is insufficient
 * space in the existing page to complete a request.  The largest single
 * memory allocation one can make is restricted by the page size used, and
 * it is best to allocate objects a significant fraction smaller than the
 * page size, as fragmentation occurs at the end of pages when there is
 * insufficient space in the current page to complete a request.
 * @author David Sugar <dyfet@gnutelephony.org>
 */
class __EXPORT mempager : public memalloc
{
private:
    pthread_mutex_t mutex;

protected:
    /**
     * Lock the memory pager mutex.  It will be more efficient to lock
     * the pager and then call the locked allocator than using alloc which
     * separately locks and unlocks for each request when a large number of
     * allocation requests are being batched together.
     */
    virtual void _lock(void);

    /**
     * Unlock the memory pager mutex.
     */
    virtual void _unlock(void);

public:
    /**
     * Construct a memory pager.
     * @param page size to use or 0 for OS allocation size.
     */
    mempager(size_t page = 0);

    /**
     * Destroy a memory pager.  Release all pages back to the heap at once.
     */
    virtual ~mempager();

    /**
     * Determine fragmentation level of acquired heap pages.  This is
     * represented as an average % utilization (0-100) and represents the
     * used portion of each allocated heap page verse the page size.  Since
     * requests that cannot fit on an already allocated page are moved into
     * a new page, there is some unusable space left over at the end of the
     * page.  When utilization approaches 100, this is good.  A low utilization
     * may suggest a larger page size should be used.
     * @return pager utilization.
     */
    unsigned utilization(void);

    /**
     * Purge all allocated memory and heap pages immediately.
     */
    void purge(void);

    /**
     * Return memory back to pager heap.  This actually does nothing, but
     * might be used in a derived class to create a memory heap that can
     * also receive (free) memory allocated from our heap and reuse it,
     * for example in a full private malloc implementation in a derived class.
     * @param memory to free back to private heap.
     */
    virtual void dealloc(void *memory);

    /**
     * Allocate memory from the pager heap.  The size of the request must be
     * less than the size of the memory page used.  This impliments the
     * memory protocol with mutex locking for thread safety.
     * is locked during this operation and then released.
     * @param size of memory request.
     * @return allocated memory or NULL if not possible.
     */
    virtual void *_alloc(size_t size);
};

/**
 * String pager for storing lists of NULL terminated strings.  This is
 * used for accumulating lists which can be destroyed all at once.
 * @author David Sugar <dyfet@gnutelephony.org>
 */
class __EXPORT stringpager : protected memalloc
{
private:
    unsigned members;
    LinkedObject *root;

public:
    /**
     * Member of string list.  This is exposed so that the list of strings
     * can be externally enumerated with linked_pointer<stringpager::member>
     * if so desired, through the begin() method.
     * @author David Sugar <dyfet@gnutelephony.org>
     */
    class __EXPORT member : public LinkedObject
    {
    private:
        const char *text;

    protected:
        friend class stringpager;

        inline void set(member *node)
            {next = node;};

        member(LinkedObject **root, const char *data);
        member(const char *data);

    public:
        inline const char *operator*() const
            {return text;};

        inline const char *get(void) const
            {return text;};
    };

    /**
     * Create a pager with a maximum page size.
     * @param size of pager allocation pages.
     */
    stringpager(size_t pagesize = 256);

    /**
     * Get the number of items in the pager string list.
     * @return number of items stored.
     */
    inline unsigned count(void)
        {return members;};

    /**
     * Get string item from list.  This is useful when stringpager is
     * passed as a pointer and hence inconvenient for the [] operator.
     * @param item to access.
     * @return pointer to text for item, or NULL if out of range.
     */
    const char *get(unsigned item);

    /**
     * Add text to list.
     * @param text to add.
     */
    void add(const char *text);

    /**
     * Add list to list.  This is a list of string pointers terminated with
     * NULL.
     * @param list of text to add.
     */
    void add(char **list);

    /**
     * Purge all members and release pager member.  The list can then
     * be added to again.
     */
    void clear(void);

    /**
     * Return specified member from pager list.  This is a convenience
     * operator.
     * @param item to access.
     * @return text of item or NULL if invalid.
     */
    inline const char *operator[](unsigned item)
        {return get(item);};

    /**
     * Get root of pager list.  This is useful for externally enumerating
     * the list of strings.
     * @return first member of list or NULL if empty.
     */
    inline stringpager::member *begin(void)
        {return static_cast<stringpager::member *>(root);};

    /**
     * Convenience operator to add to pager and auto-sort.
     * @param text to add to list.
     */
    inline void operator+=(const char *text)
        {add(text);};

    /**
     * Convenience operator to add to pager.
     * @param text to add to list.
     */
    inline stringpager& operator<<(const char *text)
        {add(text); return *this;};

    /**
     * Sort members.
     */
    void sort(void);

private:
    member *last;
};

/**
 * Directory pager is a paged string list for directory file names.
 * This protocol is used to convert a directory into a list of filenames.
 * As a protocol it offers a filtering method to select which files to
 * include in the list.
 * @author David Sugar <dyfet@gnutelephony.org>
 */
class __EXPORT DirPager : protected stringpager
{
protected:
    const char *dir;

    /**
     * Filter filenames in a derived class.
     * @param filename to filter.
     * @return true if include in final list.
     */
    virtual bool filter(const char *filename);

    /**
     * Load a directory path.
     * @param path to load.
     * @return true if valid.
     */
    bool load(const char *path);

public:
    DirPager();

    DirPager(const char *path);

    void operator=(const char *path);

    inline const char *operator*()
        {return dir;};

    inline operator bool()
        {return dir != NULL;};

    inline bool operator!()
        {return dir == NULL;};

    /**
     * Return specified filename from directory list.  This is a convenience
     * operator.
     * @param item to access.
     * @return text of item or NULL if invalid.
     */
    inline const char *operator[](unsigned item)
        {return stringpager::get(item);};

    inline const char *get(unsigned item)
        {return stringpager::get(item);};
};

/**
 * Buffered pager for storing paged strings for character protocol.
 * @author David Sugar <dyfet@gnutelephony.org>
 */
class __EXPORT bufpager : public memalloc, public CharacterProtocol
{
private:
    typedef struct cpage {
        struct cpage *next;
        char *text;
        unsigned size, used;
    } cpage_t;

    cpage_t *first, *last, *current, *freelist;
    unsigned cpos;
    unsigned long ccount;

    virtual int _getch(void);
    virtual int _putch(int code);

protected:
    virtual void *_alloc(size_t size);

public:
    /**
     * Reset pager text buffer protocol.
     */
    void reset(void);

    /**
     * Rewind to start of text buffer protocol.
     */
    void rewind(void);

    /**
     * Get total size.
     * @return number of characters in buffer.
     */
    inline unsigned long getUsed(void)
        {return ccount;};

    bufpager(size_t page = 0);
};

/**
 * Create a linked list of auto-releasable objects.  LinkedObject derived
 * objects can be created that are assigned to an autorelease object list.
 * When the autorelease object falls out of scope, all the objects listed'
 * with it are automatically deleted.
 * @author David Sugar <dyfet@gnutelephony.org>
 */
class __EXPORT autorelease
{
private:
    LinkedObject *pool;

public:
    /**
     * Create an initially empty autorelease pool.
     */
    autorelease();

    /**
     * Destroy an autorelease pool and delete member objects.
     */
    ~autorelease();

    /**
     * Destroy an autorelease pool and delete member objects.  This may be
     * used to release an existing pool programmatically when desired rather
     * than requiring the object to fall out of scope.
     */
    void release(void);

    /**
     * Add a linked object to the autorelease pool.
     * @param object to add to pool.
     */
    void operator+=(LinkedObject *object);
};

/**
 * This is a base class for objects that may be created in pager pools.
 * This is also used to create objects which can be maintained as managed
 * memory and returned to a pool.  The linked list is used when freeing
 * and re-allocating the object.  These objects are reference counted
 * so that they are returned to the pool they come from automatically
 * when falling out of scope.  This can be used to create automatic
 * garbage collection pools.
 * @author David Sugar <dyfet@gnutelephony.org>
 */
class __EXPORT PagerObject : public LinkedObject, public CountedObject
{
protected:
    friend class PagerPool;

    PagerPool *pager;

    /**
     * Create a pager object.  This is object is constructed by a PagerPool.
     */
    PagerObject();

    /**
     * Release a pager object reference.
     */
    void release(void);

    /**
     * Return the pager object back to it's originating pool.
     */
    void dealloc(void);
};

/**
 * Pager pool base class for managed memory pools.  This is a helper base
 * class for the pager template and generally is not used by itself.  If
 * different type pools are intended to use a common memory pager then
 * you will need to mixin a memory protocol object that performs
 * redirection such as the MemoryRedirect class.
 * @author David Sugar <dyfet@gnutelephony.org>
 */
class __EXPORT PagerPool : public MemoryProtocol
{
private:
    LinkedObject *freelist;
    pthread_mutex_t mutex;

protected:
    PagerPool();
    virtual ~PagerPool();

    PagerObject *get(size_t size);

public:
    /**
     * Return a pager object back to our free list.
     * @param object to return to pool.
     */
    void put(PagerObject *object);
};

class __EXPORT charmem : public CharacterProtocol
{
protected:
    char *buffer;
    size_t inp, out, size;
    bool dynamic;

    int _getch(void);
    int _putch(int code);

public:
    charmem(char *mem, size_t size);
    charmem(size_t size);
    charmem();
    virtual ~charmem();

    void release(void);

    void set(char *mem, size_t size);

    void set(size_t size);

    inline void reset(void)
        {inp = out = 0;}

    inline void rewind(void)
        {inp = 0;}
};

class __EXPORT chartext : public CharacterProtocol
{
private:
    char *pos;
    size_t max;

    int _putch(int code);
    int _getch(void);

public:
    chartext();
    chartext(char *buf);
    chartext(char *buf, size_t size);
    virtual ~chartext();
};

/**
 * A class to hold memory pointers referenced by string names.  This is
 * used to form a typeless data pointer that can be associated and
 * referenced by string/logical name.  The memory used for forming
 * the string names can itself be managed in reusable memory pools and
 * the entire structure uses it's own private pager heap.  This allows
 * new string named pointers to be added and deleted at runtime in a thread-
 * safe manner.  This might typically be used as a session id manager or for
 * symbol tables.
 * @author David Sugar <dyfet@gnutelephony.org>
 */
class __EXPORT keyassoc : protected mempager
{
private:
    /**
     * Internal paged memory residing data class for name associated pointers.
     */
    class __LOCAL keydata : public NamedObject
    {
    public:
        void *data;
        char text[8];

        keydata(keyassoc *assoc, char *id, unsigned max, unsigned bufsize);
    };

    friend class keydata;

    unsigned count;
    unsigned paths;
    size_t keysize;
    NamedObject **root;
    LinkedObject **list;

public:
    /**
     * Create a key associated memory pointer table.
     * @param indexing size for hash map.
     * @param max size of a string name if names are in reusable managed memory.
     * @param page size of memory pager.
     */
    keyassoc(unsigned indexing = 177, size_t max = 0, size_t page = 0);

    /**
     * Destroy association object.  Release all pages back to the heap.
     */
    ~keyassoc();

    /**
     * Get the number of associations we have in our object.
     * @return number of associations stored.
     */
    inline unsigned getCount(void)
        {return count;};

    /**
     * Lookup the data pointer of a string by direct operation.
     * @param name to lookup.
     * @return pointer to data or NULL if not found.
     */
    inline void *operator()(const char *name)
        {return locate(name);};

    /**
     * Purge all associations and return allocated pages to heap.
     */
    void purge(void);

    /**
     * Lookup the data pointer by the string name given.
     * @param name to lookup.
     * @return pointer to data or NULL if not found.
     */
    void *locate(const char *name);

    /**
     * Assign a name to a data pointer.  If the name exists, it is re-assigned
     * with the new pointer value, otherwise it is created.
     * @param name to assign.
     * @param pointer value to assign with name.
     * @return false if failed because name is too long for managed table.
     */
    bool assign(char *name, void *pointer);

    /**
     * Create a new name in the association table and assign it's value.
     * @param name to create.
     * @param pointer value to assign with name.
     * @return false if already exists or name is too long for managed table.
     */
    bool create(char *name, void *pointer);

    /**
     * Remove a name and pointer association.  If managed key names are used
     * then the memory allocated for the name will be re-used.
     * @param name to remove.
     * @return pointer value of the name or NULL if not found.
     */
    void *remove(const char *name);
};

/**
 * A typed template for using a key association with typed objects.
 * This essentially forms a form of "smart pointer" that is a reference
 * to specific typed objects by symbolic name.  This is commonly used as
 * for associated indexing of typed objects.
 * @author David Sugar <dyfet@gnutelephony.org>
 */
template <class T, unsigned I = 177, size_t M = 0, size_t P = 0>
class assoc_pointer : private keyassoc
{
public:
    /**
     * Construct an associated pointer hash map based on the class template.
     */
    inline assoc_pointer() : keyassoc(I, M, P) {};

    /**
     * Get the count of typed objects stored in our hash map.
     * @return typed objects in map.
     */
    inline unsigned getCount(void)
        {return keyassoc::getCount();};

    /**
     * Purge the hash map of typed objects.
     */
    inline void purge(void)
        {keyassoc::purge();};

    /**
     * Lookup a typed object by name.
     * @param name of typed object to locate.
     * @return typed object pointer or NULL if not found.
     */
    inline T *locate(const char *name)
        {return static_cast<T*>(keyassoc::locate(name));};

    /**
     * Reference a typed object directly by name.
     * @param name of typed object to locate.
     * @return typed object pointer or NULL if not found.
     */
    inline T *operator()(const char *name)
        {return locate(name);};

    /**
     * Assign a name for a pointer to a typed object.  If the name exists,
     * it is re-assigned with the new pointer value, otherwise it is created.
     * @param name to assign.
     * @param pointer of typed object to assign with name.
     * @return false if failed because name is too long for managed table.
     */
    inline bool assign(char *name, T *pointer)
        {return keyassoc::assign(name, pointer);};

    /**
     * Create a new name in the association table and assign typed object.
     * @param name to create.
     * @param pointer of typed object to assign with name.
     * @return false if already exists or name is too long for managed table.
     */
    inline bool create(char *name, T *pointer)
        {return keyassoc::create(name, pointer);};

    /**
     * Remove a name and typed pointer association.  If managed key names are
     * used then the memory allocated for the name will be re-used.
     * @param name to remove.
     */
    inline void remove(char *name)
        {keyassoc::remove(name);};

    /**
     * Access to pager utilization stats.  This is needed because we
     * inherit keyassoc privately.
     * @return pager utilization, 0-100.
     */
    inline unsigned utilization(void)
        {return mempager::utilization();};

    /**
     * Access to number of pages allocated from heap for our associated
     * index pointer.  This is needed because we inherit keyassoc
     * privately.
     * @return count of heap pages used.
     */
    inline unsigned getPages(void)
        {return mempager::getPages();};
};

/**
 * Mempager managed type factory for pager pool objects.  This is used to
 * construct a type factory that creates and manages typed objects derived
 * from PagerObject which can be managed through a private heap.
 * @author David Sugar <dyfet@gnutelephony.org>
 */
template <typename T>
class pager : private MemoryRedirect, private PagerPool
{
public:
    /**
     * Construct a pager and optionally assign a private pager heap.
     * @param heap pager to use.  If NULL, uses global heap.
     */
    inline pager(mempager *heap = NULL) : MemoryRedirect(heap), PagerPool() {};

    /**
     * Purge managed objects.
     */
    inline ~pager()
        {mempager::purge();};

    /**
     * Create a managed object by casting reference.
     * @return pointer to typed managed pager pool object.
     */
    inline T *operator()(void)
        {return new(get(sizeof(T))) T;};

    /**
     * Create a managed object by pointer reference.
     * @return pointer to typed managed pager pool object.
     */
    inline T *operator*()
        {return new(get(sizeof(T))) T;};
};

/**
 * A template class for a hash pager.  This creates objects from a pager
 * pool when they do not already exist in the hash map.
 * @author David Sugar <dyfet@gnutelephony.org>
 */
template <class T, unsigned M = 177>
class keypager : public mempager
{
private:
    NamedObject *idx[M];

public:
    /**
     * Create the object cache.
     * @param size of allocation units.
     */
    inline keypager(size_t size) : mempager(size) {};

    /**
     * Destroy the hash pager by purging the index chains and memory pools.
     */
    inline ~keypager()
        {NamedObject::purge(idx, M); mempager::purge();};

    /**
     * Find a typed object derived from NamedObject in the hash map by name.
     * If the object is not found, it is created from the memory pool.
     * @param name to search for.
     * @return typed object if found through map or NULL.
     */
    inline T *get(const char *name) const {
        T *node = (static_cast<T*>(NamedObject::map(idx, name, M)));
        if(!node) {
            node = init<T>(static_cast<T*>(mempager::_alloc(sizeof(T))));
            node->NamedObject::add(idx, name, M);
        }
        return node;
    }

    /**
     * Test if a name exists in the pool.
     * @param name to test.
     * @return true if found.
     */
    bool test(const char *name) const
        {return NamedObject::map(idx, name, M) != NULL;};

    /**
     * Find a typed object derived from NamedObject in the hash map by name.
     * If the object is not found, it is created from the pager pool.
     * @param name to search for.
     * @return typed object if found through map or NULL.
     */
    inline T *operator[](const char *name) const
        {return get(name);};

    /**
     * Find first typed object in hash map to iterate.
     * @return first typed object or NULL if nothing in list.
     */
    inline T *begin(void) const
        {return static_cast<T*>(NamedObject::skip(idx, NULL, M));};

    /**
     * Find next typed object in hash map for iteration.
     * @param current typed object we are referencing.
     * @return next iterative object or NULL if past end of map.
     */
    inline T *next(T *current) const
        {return static_cast<T*>(NamedObject::skip(idx, current, M));};

    /**
     * Count the number of typed objects in our hash map.
     * @return count of typed objects.
     */
    inline unsigned count(void) const
        {return NamedObject::count(idx, M);};

    /**
     * Convert our hash map into a linear object pointer array.  The
     * object pointer array is created from the heap and must be deleted
     * when no longer used.
     * @return array of typed named object pointers.
     */
    inline T **index(void) const
        {return NamedObject::index(idx, M);};

    /**
     * Convert our hash map into an alphabetically sorted linear object
     * pointer array.  The object pointer array is created from the heap
     * and must be deleted when no longer used.
     * @return sorted array of typed named object pointers.
     */
    inline T **sort(void) const
        {return NamedObject::sort(NamedObject::index(idx, M));};
};

/**
 * A convenience type for paged string lists.
 */
typedef stringpager stringlist_t;

/**
 * A convenience type for paged string list items.
 */
typedef stringpager::member stringlistitem_t;

/**
 * A convenience type for using DirPager directly.
 */
typedef DirPager dirlist_t;


END_NAMESPACE

#endif
