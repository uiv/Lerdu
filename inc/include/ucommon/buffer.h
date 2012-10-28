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
 * Classes which use the buffer protocol to stream data.
 * @file ucommon/buffer.h
 */

#ifndef _UCOMMON_BUFFER_H_
#define _UCOMMON_BUFFER_H_

#ifndef _UCOMMON_CONFIG_H_
#include <ucommon/platform.h>
#endif

#ifndef _UCOMMON_PROTOCOLS_H_
#include <ucommon/protocols.h>
#endif

#ifndef _UCOMMON_SOCKET_H_
#include <ucommon/socket.h>
#endif

#ifndef _UCOMMON_STRING_H_
#include <ucommon/string.h>
#endif

#ifndef _UCOMMON_FSYS_H_
#include <ucommon/fsys.h>
#endif

NAMESPACE_UCOMMON

/**
 * A generic file streaming class built from the buffer protocol.  This can
 * be used in place of fopen based file operations and does not require
 * libstdc++.
 * @author David Sugar <dyfet@gnutelephony.org>
 */
class fbuf : public BufferProtocol, private fsys
{
private:
    offset_t    inpos, outpos;

protected:
    size_t _push(const char *address, size_t size);
    size_t _pull(char *address, size_t size);
    int _err(void) const;
    void _clear(void);

    inline fd_t getfile(void)
        {return fd;};

public:
    /**
     * Construct an unopened file buffer.
     */
    fbuf();

    /**
     * Destroy object and release all resources.
     */
    ~fbuf();

    /**
     * Construct a file buffer that creates and opens a specific file.
     * @param path of file to create.
     * @param access mode of file (rw, rdonly, etc).
     * @param permissions of the newly created file.
     * @param size of the stream buffer.
     */
    fbuf(const char *path, fsys::access_t access, unsigned permissions, size_t size);

    /**
     * Construct a file buffer that opens an existing file.
     * @param path of existing file to open.
     * @param access mode of file (rw, rdonly, etc).
     * @param size of the stream buffer.
     */
    fbuf(const char *path, fsys::access_t access, size_t size);

    /**
     * Create and open the specified file.  If a file is currently open, it
     * is closed first.
     * @param path of file to create.
     * @param access mode of file (rw, rdonly, etc).
     * @param permissions of the newly created file.
     * @param size of the stream buffer.
     */
    void create(const char *path, fsys::access_t access = fsys::ACCESS_APPEND, unsigned permissions = 0640, size_t size = 512);

    /**
     * Construct a file buffer that opens an existing file.
     * @param path of existing file to open.
     * @param access mode of file (rw, rdonly, etc).
     * @param size of the stream buffer.
     */
    void open(const char *path, fsys::access_t access = fsys::ACCESS_RDWR, size_t size = 512);

    /**
     * Close the file, flush buffers.
     */
    void close(void);

    /**
     * Seek specific offset in open file and reset I/O buffers.  If the
     * file is opened for both read and write, both the read and write
     * position will be reset.
     * @param offset to seek.
     * @return true if successful.
     */
    bool seek(offset_t offset);

    /**
     * Truncate the currently open file to a specific position.  All
     * I/O buffers are reset and the file pointer is set to the end.
     * @param offset to truncate.
     * @return true if successful.
     */
    bool trunc(offset_t offset);

    /**
     * Give the current position in the currently open file.  If we are
     * appending, this is always seek::end.  If we have a file opened
     * for both read and write, this gives the read offset.
     * @return file offset of current i/o operations.
     */
    offset_t tell(void);
};

/**
 * A generic tcp socket class that offers i/o buffering.  All user i/o
 * operations are directly inherited from the IOBuffer base class public
 * members.  Some additional members are added for layering ssl services.
 * @author David Sugar <dyfet@gnutelephony.org>
 */
class __EXPORT TCPBuffer : public BufferProtocol, protected Socket
{
protected:
    void _buffer(size_t size);

    virtual size_t _push(const char *address, size_t size);
    virtual size_t _pull(char *address, size_t size);
    int _err(void) const;
    void _clear(void);
    bool _blocking(void);

    /**
     * Get the low level socket object.
     * @return socket we are using.
     */
    inline socket_t getsocket(void) const
        {return so;};

public:
    /**
     * Construct an unconnected tcp client and specify our service profile.
     */
    TCPBuffer();

    /**
     * Construct a tcp server session from a listening socket.
     * @param server socket we are created from.
     * @param size of buffer and tcp fragments.
     */
    TCPBuffer(const TCPServer *server, size_t size = 536);

    /**
     * Construct a tcp client session connected to a specific host uri.
     * @param host and optional :port we are connecting to.
     * @param service identifier of our client.
     * @param size of buffer and tcp fragments.
     */
    TCPBuffer(const char *host, const char *service, size_t size = 536);

    /**
     * Destroy the tcp socket and release all resources.
     */
    virtual ~TCPBuffer();

    /**
     * Connect a tcp socket to a client from a listener.  If the socket was
     * already connected, it is automatically closed first.
     * @param server we are connected from.
     * @param size of buffer and tcp fragments.
     */
    void open(const TCPServer *server, size_t size = 536);

    /**
     * Connect a tcp client session to a specific host uri.  If the socket
     * was already connected, it is automatically closed first.
     * @param host we are connecting.
     * @param service to connect to.
     * @param size of buffer and tcp fragments.
     */
    void open(const char *host, const char *service, size_t size = 536);

    /**
     * Close active connection.
     */
    void close(void);

protected:
    /**
     * Check for pending tcp or ssl data.
     * @return true if data pending.
     */
    virtual bool _pending(void);
};

/**
 * Convenience type for buffered file operations.
 */
typedef fbuf file_t;

/**
 * Convenience type for pure tcp sockets.
 */
typedef TCPBuffer tcp_t;

END_NAMESPACE

#endif
