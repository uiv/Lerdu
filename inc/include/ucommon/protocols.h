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
 * Abstract interfaces and support.  This is a set of "protocols", a concept
 * borrowed from other object oriented languages, to define interfaces for
 * low level services.  By using a protocol base class which offers both
 * virtuals and support methods only, one can easily stack and share these
 * as common base classes without having to consider when the final derived
 * object implements them.  Core protocol methods always are tagged with a
 * _ prefix to make it easier to track their derivation.
 * @file ucommon/protocols.h
 * @author David Sugar <dyfet@gnutelephony.org>
 */

#ifndef _UCOMMON_PROTOCOLS_H_
#define _UCOMMON_PROTOCOLS_H_

#ifndef _UCOMMON_CPR_H_
#include <ucommon/cpr.h>
#endif

NAMESPACE_UCOMMON

class string;

class __EXPORT MemoryProtocol
{
protected:
    friend class MemoryRedirect;
    /**
     * Protocol to allocate memory from the pager heap.  The size of the
     * request must be less than the size of the memory page used.  The
     * actual method is in a derived or stacked object.
     * @param size of memory request.
     * @return allocated memory or NULL if not possible.
     */
    virtual void *_alloc(size_t size) = 0;

public:
    virtual ~MemoryProtocol();

    /**
     * Convenience function.
     * @param size of memory request.
     * @return alocated memory or NULL if not possible.
     */
    inline void *alloc(size_t size)
        {return _alloc(size);};

    /**
     * Allocate memory from the pager heap.  The size of the request must be
     * less than the size of the memory page used.  The memory is initialized
     * to zero.  This uses alloc.
     * @param size of memory request.
     * @return allocated memory or NULL if not possible.
     */
    void *zalloc(size_t size);

    /**
     * Duplicate NULL terminated string into allocated memory.  This uses
     * alloc.
     * @param string to copy into memory.
     * @return allocated memory with copy of string or NULL if cannot allocate.
     */
    char *dup(const char *string);

    /**
     * Duplicate existing memory block into allocated memory.  This uses alloc.
     * @param memory to data copy from.
     * @param size of memory to allocate.
     * @return allocated memory with copy or NULL if cannot allocate.
     */
    void *dup(void *memory, size_t size);
};

/**
 * A redirection base class for the memory protocol.  This is used because
 * sometimes we choose a common memory pool to manage different objects.
 * @author David Sugar <dyfet@gnutelephony.org>
 */
class __EXPORT MemoryRedirect : public MemoryProtocol
{
private:
    MemoryProtocol *target;

public:
    MemoryRedirect(MemoryProtocol *protocol);

    virtual void *_alloc(size_t size);
};

/**
 * Common locking protocol.  This is used for objects that may internally
 * have sync'd functions, directly or in a derived class, that lock the
 * current object.  The default handlers do nothing but offer the virtuals
 * as a stub.
 * @author David Sugar <dyfet@gnutelephony.org>
 */
class __EXPORT LockingProtocol
{
protected:
    virtual void _lock(void);
    virtual void _unlock(void);

public:
    virtual ~LockingProtocol();
};

/**
 * Common character processing protocol.  This is used to access a character
 * from some type of streaming buffer or memory object.
 * @author David Sugar <dyfet@gnutelephony.org>
 */
class __EXPORT CharacterProtocol
{
protected:
    /**
     * Get the next character.
     * @return next character or EOF.
     */
    virtual int _getch(void) = 0;

    /**
     * Put the next character.
     * @param code to put.
     * @return code or EOF if cannot put.
     */
    virtual int _putch(int code) = 0;

public:
    virtual ~CharacterProtocol();

    /**
     * Get the next character.
     * @return next character or EOF.
     */
    inline int get(void)
        {return _getch();};

    /**
     * Put the next character.
     * @param code to put.
     * @return code or EOF if cannot put.
     */
    inline int put(int code)
        {return _putch(code);};
};

/**
 * Common buffer protocol class.  This is used to create objects which will
 * stream character data as needed.  This class can support bidirectional
 * streaming as may be needed for serial devices, sockets, and pipes.  The
 * buffering mechanisms are hidden from derived classes, and two virtuals
 * are used to communicate with the physical transport.
 * @author David Sugar <dyfet@gnutelephony.org>
 */
class __EXPORT BufferProtocol : public CharacterProtocol
{
public:
    typedef enum {BUF_RD, BUF_WR, BUF_RDWR} type_t;

private:
    const char *eol;
    char *buffer;
    char *input, *output;
    size_t bufsize, bufpos, insize, outsize;
    bool end;

protected:
    const char *format;

    /**
     * Construct an empty (unallocated) buffer.
     */
    BufferProtocol();

    /**
     * Construct a buffer of pre-allocated size and access type.
     * @param size of buffer to allocate.
     * @param access mode of buffer.
     */
    BufferProtocol(size_t size, type_t access = BUF_RDWR);

    /**
     * Destroy object by releasing buffer memory.
     */
    virtual ~BufferProtocol();

    /**
     * Set end of line marker.  Normally this is set to cr & lf, which
     * actually supports both lf alone and cr/lf termination of lines.
     * However, putline() will always add the full cr/lf if this mode is
     * used.  This option only effects getline() and putline().
     * @param string for eol for getline and putline.
     */
    inline void seteol(const char *string)
        {eol = string;};

    /**
     * Allocate I/O buffer(s) of specified size.  If a buffer is currently
     * allocated, it is released.
     * @param size of buffer to allocate.
     * @param access mode of buffer.
     */
    void allocate(size_t size, type_t access = BUF_RDWR);

    /**
     * Release (free) buffer memory.
     */
    void release(void);

    /**
     * Request workspace in output buffer.  This returns a pointer to
     * memory from the output buffer and advances the output position.
     * This is sometimes used for a form of zero copy write.
     * @param size of request area.
     * @return data pointer or NULL if not available.
     */
    char *request(size_t size);

    /**
     * Gather returns a pointer to contiguous input of specified size.
     * This may require moving the input data in memory.
     * @param size of gather space.
     * @return data pointer to gathered data or NULL if not available.
     */
    char *gather(size_t size);

    /**
     * Method to push buffer into physical i/o (write).  The address is
     * passed to this virtual since it is hidden as private.
     * @param address of data to push.
     * @param size of data to push.
     * @return number of bytes written, 0 on error.
     */
    virtual size_t _push(const char *address, size_t size) = 0;

    /**
     * Method to pull buffer from physical i/o (read).  The address is
     * passed to this virtual since it is hidden as private.
     * @param address of buffer to pull data into.
     * @param size of buffer area being pulled..
     * @return number of read written, 0 on error or end of data.
     */
    virtual size_t _pull(char *address, size_t size) = 0;

    /**
     * Method to get low level i/o error.
     * @return error from low level i/o methods.
     */
    virtual int _err(void) const = 0;

    /**
     * Method to clear low level i/o error.
     */
    virtual void _clear(void) = 0;

    /**
     * Return true if blocking.
     */
    virtual bool _blocking(void);

    /**
     * Check if data is pending.
     */
    virtual bool _pending(void);

    /**
     * Flush buffer to physical i/o.
     */
    virtual bool _flush(void);

    /**
     * Get current input position.  Sometimes used to help compute and report
     * a "tell" offset.
     * @return offset of input buffer.
     */
    inline size_t unread(void)
        {return bufpos;};

    /**
     * Get current output position.  Sometimes used to help compute a
     * "trunc" operation.
     */
    inline size_t unsaved(void)
        {return outsize;};

public:
    /**
     * Get memory from the buffer.  This method will become "get()" in
     * abi 4 and may become a protected method.
     * @param address of characters save from buffer.
     * @param count of characters to get from buffer.
     * @return number of characters actually copied.
     */
    size_t get(char *address, size_t count);

    /**
     * Put memory into the buffer.  If count is 0 then put as NULL
     * terminated string.  This method will become "put()" in abi 4 and
     * may become a protected method.
     * @param address of characters to put into buffer.
     * @param count of characters to put into buffer.
     * @return number of characters actually written.
     */
    size_t put(const char *address, size_t count = 0);

    /**
     * Get a character from the buffer.  If no data is available, return EOF.
     * @return character from buffer or eof.
     */
    int _getch(void);

    /**
     * Put a character into the buffer.
     * @return character put into buffer or eof.
     */
    int _putch(int ch);

    /**
     * Print formatted string to the buffer.  The maximum output size is
     * the buffer size, and the operation flushes the buffer.
     * @param format string.
     * @return number of bytes written.
     */
    size_t printf(const char *format, ...) __PRINTF(2, 3);

    /**
     * Flush buffered memory to physical I/O.
     * @return true on success, false if not active or fails.
     */
    inline bool flush(void)
        {return _flush();}

    /**
     * Purge any pending input or output buffer data.
     */
    void purge(void);

    /**
     * Reset input buffer state.  Drops any pending input.
     */
    void reset(void);

    /**
     * Get text as a line of input from the buffer.  The eol character(s)
     * are used to mark the end of a line.  Because the end of line character
     * is stripped, the length of the string may be less than the actual
     * count read.  If at the end of the file buffer and unable to read more
     * data an error occured then 0 is returned.
     * @param string to save input into.
     * @param size limit of string to save.
     * @return count of characters actually read or 0 if at end of data.
     */
    size_t getline(char *string, size_t size);

    /**
     * Get a string as a line of input from the buffer.  The eol character(s)
     * are used to mark the end of a line.  Because the end of line character
     * is stripped, the length of the string may be less than the actual
     * count read.  If at the end of the file buffer and unable to read more
     * data an error occured then 0 is returned.
     * @param buffer to save input into.
     * @return count of characters actually read or 0 if at end of data.
     */
    size_t getline(string& buffer);

    /**
     * Put a string as a line of output to the buffer.  The eol character is
     * appended to the end.
     * @param string to write.
     * @return total characters successfully written, including eol chars.
     */
    size_t putline(const char *string);

    /**
     * Check if at end of input.
     * @return true if end of data, false if input still buffered.
     */
    bool eof(void);

    /**
     * See if buffer open.
     * @return true if buffer active.
     */
    inline operator bool()
        {return buffer != NULL;}

    /**
     * See if buffer closed.
     * @return true if buffer inactive.
     */
    inline bool operator!()
        {return buffer == NULL;}

    /**
     * See if buffer open.
     * @return true if buffer active.
     */
    inline bool is_open(void)
        {return buffer != NULL;}

    /**
     * See if input active.
     * @return true if input active.
     */
    inline bool is_input(void)
        {return input != NULL;}

    /**
     * See if output active.
     * @return true if output active.
     */
    inline bool is_output(void)
        {return output != NULL;}

    /**
     * See if pending input.
     * @return true if input pending.
     */
    inline bool is_pending(void)
        {return _pending();}

    /**
     * Set eof flag.
     */
    inline void seteof(void)
        {end = true;}

    inline int err(void)
        {return _err();}
};

END_NAMESPACE

#endif
