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
 * Thread-aware file system manipulation class.  This is used to provide
 * generic file operations that are OS independent and thread-safe in
 * behavior.  This is used in particular to wrap posix calls internally
 * to pth, and to create portable code between MSWINDOWS and Posix low-level
 * file I/O operations.
 * @file ucommon/fsys.h
 */

#ifndef _UCOMMON_FILE_H_
#define _UCOMMON_FILE_H_

#ifndef _UCOMMON_CONFIG_H_
#include <ucommon/platform.h>
#endif

#ifndef _UCOMMON_PROTOCOLS_H_
#include <ucommon/protocols.h>
#endif

#ifndef _UCOMMON_THREAD_H_
#include <ucommon/thread.h>
#endif

#ifndef _UCOMMON_STRING_H_
#include <ucommon/string.h>
#endif

#ifndef _MSWINDOWS_
#include <sys/stat.h>
#else
#include <io.h>
#ifndef R_OK
#define F_OK 0
#define X_OK 1
#define W_OK 2
#define R_OK 4
#endif
#endif

#include <errno.h>
#include <stdio.h>

#ifndef __S_ISTYPE
#define __S_ISTYPE(mode, mask)  (((mode) & S_IFMT) == (mask))
#endif

#if !defined(S_ISDIR) && defined(S_IFDIR)
#define S_ISDIR(mode)   __S_ISTYPE((mode), S_IFDIR)
#endif

#if !defined(S_ISCHR) && defined(S_IFCHR)
#define S_ISCHR(mode)   __S_ISTYPE((mode), S_IFCHR)
#elif !defined(S_ISCHR)
#define S_ISCHR(mode)   0
#endif

#if !defined(S_ISBLK) && defined(S_IFBLK)
#define S_ISBLK(mode)   __S_ISTYPE((mode), S_IFBLK)
#elif !defined(S_ISBLK)
#define S_ISBLK(mode)   0
#endif

#if !defined(S_ISREG) && defined(S_IFREG)
#define S_ISREG(mode)   __S_ISTYPE((mode), S_IFREG)
#elif !defined(S_ISREG)
#define S_ISREG(mode)   1
#endif

#if !defined(S_ISSOCK) && defined(S_IFSOCK)
#define S_ISSOCK(mode)  __S_ISTYPE((mode), S_IFSOCK)
#elif !defined(S_ISSOCK)
#define S_ISSOCK(mode)  (0)
#endif

#if !defined(S_ISFIFO) && defined(S_IFIFO)
#define S_ISFIFO(mode)  __S_ISTYPE((mode), S_IFIFO)
#elif !defined(S_ISFIFO)
#define S_ISFIFO(mode)  (0)
#endif

#if !defined(S_ISLNK) && defined(S_IFLNK)
#define S_ISLNK(mode)   __S_ISTYPE((mode), S_IFLNK)
#elif !defined(S_ISLNK)
#define S_ISLNK(mode)   (0)
#endif

NAMESPACE_UCOMMON

/**
 * Convenience type for directory scan operations.
 */
typedef void *dir_t;

/**
 * Convenience type for loader operations.
 */
typedef void *mem_t;

/**
 * A container for generic and o/s portable threadsafe file system functions.
 * These are based roughly on their posix equivilents.  For libpth, the
 * system calls are wrapped.  The native file descriptor or handle may be
 * used, but it is best to use "class fsys" instead because it can capture
 * the errno of a file operation in a threadsafe and platform independent
 * manner, including for mswindows targets.
 */
class __EXPORT fsys
{
protected:
    fd_t    fd;
#ifdef  _MSWINDOWS_
    WIN32_FIND_DATA *ptr;
    HINSTANCE   mem;
#else
    void    *ptr;
#endif
    int     error;

public:
    typedef struct stat fileinfo_t;

#ifdef  _MSWINDOWS_
    static int remapError(void);
#else
    inline static int remapError(void)
        {return errno;};
#endif

    /**
     * Enumerated file access modes.
     */
    typedef enum {
        ACCESS_RDONLY,
        ACCESS_WRONLY,
        ACCESS_REWRITE,
        ACCESS_RDWR = ACCESS_REWRITE,
        ACCESS_APPEND,
        ACCESS_SHARED,
        ACCESS_DIRECTORY,
        ACCESS_STREAM,
        ACCESS_RANDOM
    } access_t;

    /**
     * File offset type.
     */
    typedef long offset_t;

    /**
     * Used to mark "append" in set position operations.
     */
    static const offset_t end;

    /**
     * Construct an unattached fsys descriptor.
     */
    fsys();

    /**
     * Contruct fsys from raw file handle.
     */
    fsys(fd_t handle);

    /**
     * Copy (dup) an existing fsys descriptor.
     * @param descriptor to copy from.
     */
    fsys(const fsys& descriptor);

    /**
     * Create a fsys descriptor by opening an existing file or directory.
     * @param path of file to open for created descriptor.
     * @param access mode of file.
     */
    fsys(const char *path, access_t access);

    /**
     * Create a fsys descriptor by creating a file.
     * @param path of file to create for descriptor.
     * @param access mode of file access.
     * @param permission mode of file.
     */
    fsys(const char *path, access_t access, unsigned permission);

    /**
     * Close and release a file descriptor.
     */
    ~fsys();

    /**
     * Get the descriptor from the object by pointer reference.
     * @return low level file handle.
     */
    inline fd_t operator*() const
        {return fd;};

    /**
     * Get the descriptor from the object by casting reference.
     * @return low level file handle.
     */
    inline operator fd_t() const
        {return fd;};

    /**
     * Test if file descriptor is open.
     * @return true if open.
     */
    inline operator bool() const
        {return fd != INVALID_HANDLE_VALUE || ptr != NULL;};

    /**
     * Test if file descriptor is closed.
     * @return true if closed.
     */
    inline bool operator!() const
        {return fd == INVALID_HANDLE_VALUE && ptr == NULL;};

    /**
     * Assign file descriptor by duplicating another descriptor.
     * @param descriptor to dup from.
     */
    void operator=(const fsys& descriptor);

    /**
     * Assing file descriptor from system descriptor.
     * @param descriptor to dup from.
     */
    void operator=(fd_t descriptor);

    /**
     * Get the native system descriptor handle of the file descriptor.
     * @return native os descriptor.
     */
    inline fd_t getHandle(void) const
        {return fd;};

    /**
     * Set with external descriptor.  Closes existing file if open.
     * @param descriptor of open file.
     */
    void set(fd_t descriptor);

    /**
     * Release descriptor, do not close.
     * @return handle being released.
     */
    fd_t release(void);

    /**
     * Set the position of a file descriptor.
     * @param offset from start of file or "end" to append.
     * @return error number or 0 on success.
     */
    int seek(offset_t offset);

    /**
     * Drop cached data from start of file.
     * @param size of region to drop or until end of file.
     * @return error number or 0 on success.
     */
    int drop(offset_t size = 0);

    /**
     * See if current file stream is a tty device.
     * @return true if device.
     */
    bool istty(void);

    /**
     * See if the file handle is a tty device.
     * @return true if device.
     */
    static bool istty(fd_t fd);

    /**
     * Read data from descriptor or scan directory.
     * @param buffer to read into.
     * @param count of bytes to read.
     * @return bytes transferred, -1 if error.
     */
    ssize_t read(void *buffer, size_t count);

    /**
     * Write data to descriptor.
     * @param buffer to write from.
     * @param count of bytes to write.
     * @return bytes transferred, -1 if error.
     */
    ssize_t write(const void *buffer, size_t count);

    /**
     * Get status of open descriptor.
     * @param buffer to save status info in.
     * @return error number or 0 on success.
     */
    int fileinfo(fileinfo_t *buffer);

    /**
     * Truncate file to specified length.  The file pointer is positioned
     * to the new end of file.
     * @param offset to truncate to.
     * @return true if truncate successful.
     */
    int trunc(offset_t offset);

    /**
     * Commit changes to the filesystem.
     * @return error number or 0 on success.
     */
    int sync(void);

    /**
     * Set directory prefix (chdir).
     * @param path to change to.
     * @return error number or 0 on success.
     */
    static int changeDir(const char *path);

    /**
     * Get current directory prefix (pwd).
     * @param path to save directory into.
     * @param size of path we can save.
     * @return error number or 0 on success.
     */
    static int getPrefix(char *path, size_t size);

    /**
     * Stat a file.
     * @param path of file to stat.
     * @param buffer to save stat info.
     * @return error number or 0 on success.
     */
    static int fileinfo(const char *path, fileinfo_t *buffer);

    /**
     * Remove a file.
     * @param path of file.
     * @return error number or 0 on success.
     */
    static int remove(const char *path);

    /**
     * Copy a file.
     * @param source file.
     * @param target file.
     * @param size of buffer.
     * @return error number or 0 on success.
     */
    static int copy(const char *source, const char *target, size_t size = 1024);

    /**
     * Rename a file.
     * @param oldpath to rename from.
     * @param newpath to rename to.
     * @return error number or 0 on success.
     */
    static int rename(const char *oldpath, const char *newpath);

    /**
     * Change file access mode.
     * @param path to change.
     * @param mode to assign.
     * @return error number or 0 on success.
     */
    static int change(const char *path, unsigned mode);

    /**
     * Test path access.
     * @param path to test.
     * @param mode to test for.
     * @return error number or 0 on success.
     */
    static int access(const char *path, unsigned mode);

    /**
     * Test if path is a file.
     * @param path to test.
     * @return true if exists and is file.
     */
    static bool isfile(const char *path);

    /**
     * Test if path is a directory.
     * @param path to test.
     * @return true if exists and is directory.
     */
    static bool isdir(const char *path);

    /**
     * Test if path is a symlink.
     * @param path to test.
     * @return true if exists and is symlink.
     */
    static bool islink(const char *path);

    /**
     * Test if path is a hidden file.
     * @param path to test.
     * @return true if exists and is hidden.
     */
    static bool ishidden(const char *path);

    /**
     * Read data from file descriptor or directory.
     * @param descriptor to read from.
     * @param buffer to read into.
     * @param count of bytes to read.
     * @return bytes transferred, -1 if error.
     */
    inline static ssize_t read(fsys& descriptor, void *buffer, size_t count)
        {return descriptor.read(buffer, count);};

    /**
     * write data to file descriptor.
     * @param descriptor to write to.
     * @param buffer to write from.
     * @param count of bytes to write.
     * @return bytes transferred, -1 if error.
     */
    inline static ssize_t write(fsys& descriptor, const void *buffer, size_t count)
        {return descriptor.write(buffer, count);};

    /**
     * Set the position of a file descriptor.
     * @param descriptor to set.
     * @param offset from start of file or "end" to append.
     * @return error number or 0 on success.
     */
    inline static int seek(fsys& descriptor, offset_t offset)
        {return descriptor.seek(offset);};

    /**
     * Drop cached data from a file descriptor.
     * @param descriptor to set.
     * @param size of region from start of file to drop or all.
     * @return error number or 0 on success.
     */
    inline static int drop(fsys& descriptor, offset_t size)
        {return descriptor.drop(size);};

    /**
     * Open a file or directory.
     * @param path of file to open.
     * @param access mode of descriptor.
     */
    void open(const char *path, access_t access);

    /**
     * Assign descriptor directly.
     * @param descriptor to assign.
     */
    inline void assign(fd_t descriptor)
        {close(); fd = descriptor;};

    /**
     * Assign a descriptor directly.
     * @param object to assign descriptor to.
     * @param descriptor to assign.
     */
    inline static void assign(fsys& object, fd_t descriptor)
        {object.close(); object.fd = descriptor;};

    /**
     * Open a file descriptor directly.
     * @param path of file to create.
     * @param access mode of descriptor.
     * @param mode of file if created.
     */
    void create(const char *path, access_t access, unsigned mode);

    /**
     * Simple direct method to create a directory.
     * @param path of directory to create.
     * @param mode of directory.
     * @return error number or 0 on success.
     */
    static int createDir(const char *path, unsigned mode);

    /**
     * Simple direct method to remove a directory.
     * @param path to remove.
     * @return error number or 0 on success.
     */
    static int removeDir(const char *path);

    /**
     * Remove a symbolic link explicitly.  Other kinds of files are also
     * deleted.  This should be used when uncertain about symlinks requiring
     * special support.
     * @param path to remove.
     * @return error number or 0 on success.
     */
    static int unlink(const char *path);

    /**
     * Create a symbolic link.
     * @param path to create.
     * @param target of link.
     * @return error number or 0 on success.
     */
    static int link(const char *path, const char *target);

    /**
     * Create a hard link.
     * @param path to create link to.
     * @param target of link.
     * @return error number or 0 on success.
     */
    static int hardlink(const char *path, const char *target);

    /**
     * Read a symbolic link to get it's target.
     * @param path of link.
     * @param buffer to save target into.
     * @param size of buffer.
     */
    static int linkinfo(const char *path, char *buffer, size_t size);

    /**
     * Close a file descriptor or directory directly.
     * @param descriptor to close.
     */
    inline static void close(fsys& descriptor)
        {descriptor.close();};

    /**
     * Close a fsys resource.
     */
    void close(void);

    /**
     * Get last error.
     * @return error number.
     */
    inline int err(void) const
        {return error;}

    /**
     * Open a file or directory.
     * @param object to assign descriptor for opened file.
     * @param path of file to open.
     * @param access mode of descriptor.
     */
    inline static void open(fsys& object, const char *path, access_t access)
        {object.open(path, access);};

    /**
     * Direct means to open a read-only file path and return a descriptor.
     * @param path to open.
     * @return descriptor on success, invalid handle on failure.
     */
    static fd_t input(const char *path);

    /**
     * Direct means to create or access a writable path and return descriptor.
     * @param path to create.
     * @return descriptor on success, invalid handle on failure.
     */
    static fd_t output(const char *path);

    /**
     * Direct means to create or append a writable path and return descriptor.
     * @param path to create.
     * @return descriptor on success, invalid handle on failure.
     */
    static fd_t append(const char *path);

    /**
     * Release a file descriptor.
     * @param descriptor to release.
     */
    static void release(fd_t descriptor);

    /**
     * create a file descriptor or directory directly.
     * @param object to assign descriptor for created file.
     * @param path of file to create.
     * @param access mode of descriptor.
     * @param mode of file if created.
     */
    inline static void create(fsys& object, const char *path, access_t access, unsigned mode)
        {object.create(path, access, mode);};

    /**
     * Load an unmaged plugin directly.
     * @param path to plugin.
     * @return error number or 0 on success.
     */
    static int load(const char *path);

    /**
     * Load a plugin into memory.
     * @param module for management.
     * @param path to plugin.
     */
    static void load(fsys& module, const char *path);

    /**
     * unload a specific plugin.
     * @param module to unload
     */
    static void unload(fsys& module);

    /**
     * Find symbol in loaded module.
     * @param module to search.
     * @param symbol to search for.
     * @return address of symbol or NULL if not found.
     */
    static void *find(fsys& module, const char *symbol);

    static inline bool isfile(struct stat *inode)
        {return S_ISREG(inode->st_mode);}

    static inline bool isdir(struct stat *inode)
        {return S_ISDIR(inode->st_mode);}

    static inline bool islink(struct stat *inode)
        {return S_ISLNK(inode->st_mode);}

    static inline bool isdev(struct stat *inode)
        {return S_ISBLK(inode->st_mode) || S_ISCHR(inode->st_mode);}

    static inline bool ischar(struct stat *inode)
        {return S_ISCHR(inode->st_mode);}

    static inline bool isdisk(struct stat *inode)
        {return S_ISBLK(inode->st_mode);}

    static inline bool issys(struct stat *inode)
        {return S_ISSOCK(inode->st_mode) || S_ISFIFO(inode->st_mode);}
};

/**
 * Access standard files through character protocol.  This can also be
 * used as an alternative means to access files that manages file pointers.
 * @author David Sugar <dyfet@gnutelephony.org>
 */
class __EXPORT charfile : public CharacterProtocol
{
private:
    FILE *fp;
    bool opened;

    int _putch(int code);

    int _getch(void);

public:
    typedef ::fpos_t bookmark_t;

    /**
     * Construct a charfile from an existing FILE pointer.
     * @param file to use.
     */
    inline charfile(FILE *file)
        {fp = file; opened = false;}

    /**
     * Construct an open charfile based on a path and mode.
     * @param path of file to open.
     * @param mode of file.
     */
    charfile(const char *path, const char *mode);

    /**
     * Construct an unopened file.
     */
    charfile();

    /**
     * Destroy object and close associated file.
     */
    ~charfile();

    /**
     * Test if file is opened.
     * @return true if opened.
     */
    inline operator bool()
        {return fp != NULL;}

    /**
     * Test if file is not opened.
     * @return true if not opened.
     */
    inline bool operator !()
        {return fp == NULL;}

    /**
     * Open file path.  If a file is already opened, it is closed.
     * @param path of file to open.
     * @param mode of file to open.
     */
    void open(const char *path, const char *mode);

    /**
     * Close an open file.
     */
    void close(void);

    /**
     * Put a string into the file.
     * @param string to write.
     * @return number of characters written.
     */
    size_t put(const char *string);

    /**
     * Read a line of input from the file.  This clears the newline
     * character at the end and has consistent behavior with other
     * ucommon file routines.  Because the newline is cleared, the
     * string length may be shorter than the return size.
     * @param string to write.
     * @param size of buffer.
     * @return true if data read, 0 if at end of file.
     */
    size_t readline(char *string, size_t size);

    /**
     * Read a string of input from the file.  This clears the newline
     * character at the end and has consistent behavior with other
     * ucommon file routines.  Because the newline is cleared, the
     * string length may be shorter than the return size.
     * @param string to write.
     * @return true if data read, 0 if at end of file.
     */
    size_t readline(string& string);

    inline size_t put(const void *data, size_t size)
        { return fp == NULL ? 0 : fwrite(data, 1, size, fp);}

    size_t get(void *data, size_t size)
        { return fp == NULL ? 0 : fread(data, 1, size, fp);}

    inline void get(bookmark_t& pos)
        { if(fp) fsetpos(fp, &pos);}

    inline void set(bookmark_t& pos)
        { if(fp) fgetpos(fp, &pos);}

    int err(void);

    bool eof(void);

    inline void seek(long offset)
        {if(fp) fseek(fp, offset, SEEK_SET);}

    inline void move(long offset)
        {if(fp) fseek(fp, offset, SEEK_CUR);}

    inline void append(void)
        {if (fp) fseek(fp, 0l, SEEK_END);}

    inline void rewind(void)
        {if(fp) ::rewind(fp);}

    size_t printf(const char *format, ...) __PRINTF(2, 3);

    bool istty(void);
};

String str(charfile& fp, strsize_t size);

/**
 * Convience type for fsys.
 */
typedef fsys fsys_t;

extern charfile cstdin, cstdout, cstderr;

END_NAMESPACE

#endif

