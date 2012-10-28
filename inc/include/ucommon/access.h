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
 * Locking classes for member function automatic operations.
 * This header covers ucommon access related classes.  These are used to
 * provide automatic management of locks and synchronization objects through
 * common virtual base classes which can be used with automatic objects.
 * These classes are intended to be used much like "protocols" in conjunction
 * with smart pointer/referencing classes.  This protocol interface supports
 * member functions to acquire a lock when entered and automatically
 * release the lock when the member function returns that are used in
 * conjunction with special referencing smart pointers.
 * @file ucommon/access.h
 */

// we do this twice because of some bizarre issue in just this file that
// otherwise breaks doxygen and lists all items outside the namespace...
#include <ucommon/platform.h>

#ifndef _UCOMMON_ACCESS_H_
#define _UCOMMON_ACCESS_H_

#ifndef _UCOMMON_CPR_H_
#include <ucommon/cpr.h>
#endif

NAMESPACE_UCOMMON

/**
 * An exclusive locking protocol interface base.
 * This is an abstract class to form objects that will operate under an
 * exclusive lock while being actively referenced by a smart pointer.
 * @author David Sugar <dyfet@gnutelephony.org>
 */
class __EXPORT ExclusiveProtocol
{
protected:
    virtual ~ExclusiveProtocol();

public:
    /**
     * Protocol interface to exclusive lock the object.
     */
    virtual void Exlock(void) = 0;

    /**
     * Protocol interface to release a lock.
     */
    virtual void Unlock(void) = 0;

    /**
     * A convenience member function for accessing the exclusive lock.
     */
    inline void Lock(void)
        {Exlock();};
};

/**
 * An exclusive locking protocol interface base.
 * This is an abstract class to form objects that will operate under an
 * exclusive lock while being actively referenced by a smart pointer.
 * @author David Sugar <dyfet@gnutelephony.org>
 */
class __EXPORT SharedProtocol
{
protected:
    virtual ~SharedProtocol();

public:
    /**
     * Protocol interface to share lock the object.
     */
    virtual void Shlock(void) = 0;

    /**
     * Protocol interface to release a lock.
     */
    virtual void Unlock(void) = 0;

    /**
     * Share the lock with other referencers.  Many of our shared locking
     * objects support the ability to switch between shared and exclusive
     * mode.  This derived protocol member allows one to restore the lock
     * to shared mode after it has been made exclusive.
     */
    virtual void Share(void);

    /**
     * Convert object to an exclusive lock.  Many of our shared locking
     * objects such as the "conditional lock" support the ability to switch
     * between shared and exclusive locking modes.  This derived protocol
     * member allows one to temporarily assert exclusive locking when tied
     * to such methods.
     */
    virtual void Exclusive(void);

    /**
     * A convenience member function for accessing the shared lock.
     */
    inline void Lock(void)
        {Shlock();};
};

/**
 * A kind of smart pointer object to support exclusive locking protocol.
 * This object initiates an exclusive lock for the object being referenced when
 * it is instantiated, and releases the exclusive lock when it is destroyed.
 * You would pass the pointer an object that has the Exclusive as a base class.
 * @author David Sugar <dyfet@gnutelephony.org>
 */
class __EXPORT exclusive_lock
{
private:
    ExclusiveProtocol *lock;

public:
    /**
     * Create an instance of an exclusive object reference.
     * @param object containing Exclusive base class protocol to lock.
     */
    exclusive_lock(ExclusiveProtocol *object);

    /**
     * Destroy reference to exclusively locked object, release lock.
     */
    ~exclusive_lock();

    /**
     * Test if the reference holds an active lock.
     * @return true if is not locking an object.
     */
    inline bool operator!() const
        {return lock == NULL;};

    /**
     * Test if the reference holds an active lock.
     * @return true if locking an object.
     */
    inline operator bool() const
        {return lock != NULL;};

    /**
     * Release a held lock programmatically.  This can be used to de-reference
     * the object being exclusively locked without having to wait for the
     * destructor to be called when the exclusive_lock falls out of scope.
     */
    void release(void);
};

/**
 * A kind of smart pointer object to support shared locking protocol.
 * This object initiates a shared lock for the object being referenced when
 * it is instantiated, and releases the shared lock when it is destroyed.
 * You would pass the pointer an object that has the Shared as a base class.
 * @author David Sugar <dyfet@gnutelephony.org>
 */
class __EXPORT shared_lock
{
private:
    SharedProtocol *lock;
    int state;
    bool modify;

public:
    /**
     * Create an instance of an exclusive object reference.
     * @param object containing Exclusive base class protocol to lock.
     */
    shared_lock(SharedProtocol *object);

    /**
     * Destroy reference to shared locked object, release lock.
     */
    ~shared_lock();

    /**
     * Test if the reference holds an active lock.
     * @return true if is not locking an object.
     */
    inline bool operator!() const
        {return lock == NULL;};

    /**
     * Test if the reference holds an active lock.
     * @return true if locking an object.
     */
    inline operator bool() const
        {return lock != NULL;};

    /**
     * Release a held lock programmatically.  This can be used to de-reference
     * the object being share locked without having to wait for the
     * destructor to be called when the shared_lock falls out of scope.
     */
    void release(void);

    /**
     * Call exclusive access on referenced objects protocol.
     */
    void exclusive(void);

    /**
     * Restore shared access on referenced objects protocol.
     */
    void share(void);
};

/**
 * Convenience function to exclusively lock an object through it's protocol.
 * @param object to lock.
 */
inline void lock(ExclusiveProtocol *object)
    {object->Exlock();}

/**
 * Convenience function to unlock an exclusive object through it's protocol.
 * @param object to unlock.
 */
inline void unlock(ExclusiveProtocol *object)
    {object->Unlock();}

/**
 * Convenience function to access (lock) shared object through it's protocol.
 * @param object to share lock.
 */
inline void access(SharedProtocol *object)
    {object->Shlock();}

/**
 * Convenience function to unlock shared object through it's protocol.
 * @param object to unlock.
 */
inline void release(SharedProtocol *object)
    {object->Unlock();}

/**
 * Convenience function to exclusive lock shared object through it's protocol.
 * @param object to exclusive lock.
 */
inline void exclusive(SharedProtocol *object)
    {object->Exclusive();}

/**
 * Convenience function to restore shared locking for object through it's protocol.
 * @param object to restore shared locking.
 */
inline void share(SharedProtocol *object)
    {object->Share();}

/**
 * Convenience type to use for object referencing an exclusive object.
 */
typedef exclusive_lock exlock_t;

/**
 * Convenience type to use for object referencing a shared object.
 */
typedef shared_lock shlock_t;

/**
 * Convenience function to release a reference to an exclusive lock.
 * @param reference to object referencing exclusive locked object.
 */
inline void release(exlock_t &reference)
    {reference.release();}

/**
 * Convenience function to release a reference to a shared lock.
 * @param reference to object referencing shared locked object.
 */
inline void release(shlock_t &reference)
    {reference.release();}

// Special macros to allow member functions of an object with a protocol
// to create self locking states while the member functions are called by
// placing an exclusive_lock or shared_lock smart object on their stack
// frame to reference their self.

#define exclusive_object()  exlock_t __autolock__ = this
#define protected_object()  shlock_t __autolock__ = this
#define exclusive_access(x) exlock_t __autolock__ = &x
#define protected_access(x) shlock_t __autolock__ = &x

END_NAMESPACE

#endif
