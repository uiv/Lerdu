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
 * Generic shell parsing and application services.
 * @file ucommon/shell.h
 */

/**
 * Example of shell parsing.
 * @example shell.cpp
 */

#ifndef _UCOMMON_STRING_H_
#include <ucommon/string.h>
#endif

#ifndef _UCOMMON_MEMORY_H_
#include <ucommon/memory.h>
#endif

#ifndef _UCOMMON_BUFFER_H_
#include <ucommon/buffer.h>
#endif

#ifndef _UCOMMON_SHELL_H_
#define _UCOMMON_SHELL_H_

#ifdef  _MSWINDOWS_
#define INVALID_PID_VALUE   INVALID_HANDLE_VALUE
#else
#define INVALID_PID_VALUE   -1
#endif

#ifdef  ERR
#undef  ERR
#endif

NAMESPACE_UCOMMON

/**
 * A utility class for generic shell operations.  This includes utilities
 * to parse and expand arguments, and to call system shell services.  This
 * also includes a common shell class to parse and process command line
 * arguments which are managed through a local heap.
 * @author David Sugar <dyfet@gnutelephony.org>
 */
class __EXPORT shell : public mempager
{
private:
    char **_argv;
    unsigned _argc;
    char *_argv0;
    char *_exedir;
    LinkedObject *_syms;

    class __LOCAL args : public OrderedObject
    {
    public:
        char *item;
    };

    class __LOCAL syms : public LinkedObject
    {
    public:
        const char *name;
        const char *value;
    };

    /**
     * Collapse argument list.  This is used internally to collapse args
     * that are created in a pool heap when they need to be turned into
     * an argv style array.
     */
    void collapse(LinkedObject *first);

    /**
     * Set argv0.  This gets the simple filename of argv[0].
     */
    void set0(char *argv0);

public:
    /**
     * Error table index.
     */
    typedef enum {NOARGS = 0, NOARGUMENT, INVARGUMENT, BADOPTION, OPTION_USED, BAD_VALUE, NUMERIC_SET} errmsg_t;

    /**
     * Type of error logging we are using.
     */
    typedef enum {NONE = 0, CONSOLE_LOG, USER_LOG, SYSTEM_LOG, SECURITY_LOG} logmode_t;

    /**
     * Level of error logging.
     */
    typedef enum {FAIL = 0, ERR, WARN, NOTIFY, INFO, DEBUG0} loglevel_t;

    /**
     * Numeric mode of parser.
     */
    typedef enum {NO_NUMERIC, NUMERIC_PLUS, NUMERIC_DASH, NUMERIC_ALL} numeric_t;

    /**
     * Path types to retrieve.
     */
    typedef enum {
        PROGRAM_CONFIG, SERVICE_CONFIG, USER_DEFAULTS, SERVICE_CONTROL,
        USER_HOME = USER_DEFAULTS + 3, SERVICE_DATA, SYSTEM_TEMP, USER_CACHE,
        SERVICE_CACHE, USER_DATA, USER_CONFIG, SYSTEM_CFG, SYSTEM_ETC,
        SYSTEM_VAR, SYSTEM_PREFIX, SYSTEM_SHARE, PROGRAM_PLUGINS,
        PROGRAM_TEMP} path_t;

    /**
     * Log process handler.
     */
    typedef bool (*logproc_t)(loglevel_t level, const char *text);

    /**
     * Main handler.
     */
    typedef cpr_service_t   mainproc_t;

    /**
     * Exit handler.
     */
    typedef void (*exitproc_t)(void);

#ifdef  _MSWINDOWS_
    typedef HANDLE pid_t;
#else
    /**
     * Standard type of process id for shell class.
     */
    typedef int pid_t;
#endif

    /**
     * Pipe I/O mode.
     */
    typedef enum {RD = BufferProtocol::BUF_RD, WR = BufferProtocol::BUF_WR, RDWR = BufferProtocol::BUF_RDWR} pmode_t;

    /**
     * A class to control a process that is piped.  This holds the active
     * file descriptors for the pipe as well as the process id.  Basic I/O
     * methods are provided to send and receive data with the piped child
     * process.  This may be used by itself with various shell methods as
     * a pipe_t, or to construct piped objects such as iobuf.
     * @author David Sugar <dyfet@gnutelephony.org>
     */
    class __EXPORT pipeio
    {
    protected:
        friend class shell;

        /**
         * Construct an empty initialized pipe for use.
         */
        pipeio();

        /**
         * Spawn and attach child process I/O through piping.  Stderr is left
         * attached to the console.
         * @param path of program to execute.  If simple file, $PATH is used.
         * @param argv to pass to child process.
         * @param mode of pipe operation; rdonly, wronly, or rdwr.
         * @param size of atomic pipe buffer if setable.
         * @param env that may optionally be given to the child process.
         */
        int spawn(const char *path, char **argv, pmode_t mode, size_t size = 512, char **env = NULL);

        /**
         * Wait for child process to exit.  When it does, close all piping.
         * @return process exit code or -1 if error.
         */
        int wait(void);

        /**
         * Signal termination and wait for child process to exit.  When it
         * does all piping is closed.
         * @return process exit code or -1 if error.
         */
        int cancel(void);

        /**
         * Read input from child process.  If there is an error, the result
         * is 0 and perror holds the error code.  If an error already
         * happened no further data will be read.
         * @param address to store input.
         * @param size of input to read.
         * @return number of bytes actually read.
         */
        size_t read(void *address, size_t size);

        /**
         * Write to the child process.  If there is an error, the result
         * is 0 and perror holds the error code.  If an error already
         * happened no further data will be written.
         * @param address to write data from.
         * @param size of data to write.
         * @return number of bytes actually written.
         */
        size_t write(const void *address, size_t size);

        pid_t pid;
        fd_t input, output; // input to and output from child process...
        int perror, presult;
    };

    /**
     * Process pipe with I/O buffering.  This allows the creation and management
     * of a shell pipe with buffered I/O support.  This also offers a common
     * class to manage stdio sessions generically in the child process.
     * @author David Sugar <dyfet@gnutelephony.org>
     */
    class __EXPORT iobuf : public BufferProtocol, private pipeio
    {
    protected:
        friend class shell;

        int ioerror;

        virtual int _err(void) const;
        virtual void _clear(void);

        virtual size_t _push(const char *address, size_t size);
        virtual size_t _pull(char *address, size_t size);

    public:
        /**
         * Construct an i/o buffer. If a non-zero size is specified, then
         * the object is attached to the process's stdin & stdout.  Otherwise
         * an un-opened object is created.
         */
        iobuf(size_t size = 0);

        /**
         * Construct an i/o buffer for a child process.  This is used to
         * create a child process directly when the object is made.  It
         * essentially is the same as the open() method.
         * @param path of program to execute, if filename uses $PATH.
         * @param argv to pass to child process.
         * @param mode of pipe, rdonly, wronly, or rdwr.
         * @param size of buffering, and atomic pipe size if settable.
         * @param env that may be passed to child process.
         */
        iobuf(const char *path, char **argv, pmode_t mode, size_t size = 512, char **env = NULL);

        /**
         * Destroy i/o buffer.  This may cancel and block waiting for a child
         * process to terminate.
         */
        ~iobuf();

        /**
         * Open the i/o buffer attached to a child process.
         * @param path of program to execute, if filename uses $PATH.
         * @param argv to pass to child process.
         * @param mode of pipe, rdonly, wronly, or rdwr.
         * @param size of buffering, and atomic pipe size if settable.
         * @param env that may be passed to child process.
         */
        void open(const char *path, char **argv, pmode_t mode, size_t size = 512, char **env = NULL);

        /**
         * Close the i/o buffer. If attached to a child process it will wait
         * for the child to exit.
         */
        void close(void);

        /**
         * Terminate child process.  This also waits for the child process
         * to exit and then closes buffers.
         */
        void cancel(void);
    };

    /**
     * Buffered i/o type for pipes and stdio.
     */
    typedef iobuf io_t;

    /**
     * Convenience low level pipe object type.
     */
    typedef pipeio *pipe_t;

    /**
     * This can be used to get internationalized error messages.  The internal
     * text for shell parser errors are passed through here.
     * @param id of error message to use.
     * @return published text of error message.
     */
    static const char *errmsg(errmsg_t id);

    /**
     * This is used to set internationalized error messages for the shell
     * parser.
     * @param id of message to set.
     * @param text for error message.
     */
    static void errmsg(errmsg_t id, const char *text);

    /**
     * A class to redefine error messages.  This can be used as a statically
     * initialized object to remap error messages for easier
     * internationalization.
     * @author David Sugar <dyfet@gnutelephony.org>
     */
    class __EXPORT errormap
    {
    public:
        inline errormap(errmsg_t id, const char *text)
            {shell::errmsg(id, text);};
    };

    /**
     * A base class used to create parsable shell options.  The virtual
     * is invoked when the shell option is detected.  Both short and long
     * forms of argument parsing are supported.  An instance of a derived
     * class is created to perform the argument parsing.
     * @author David Sugar <dyfet@gnutelephony.org>
     */
    class __EXPORT Option : public OrderedObject
    {
    public:
        char short_option;
        const char *long_option;
        const char *uses_option;
        const char *help_string;
        bool trigger_option;

        /**
         * Construct a shell parser option.
         * @param short_option for single character code.
         * @param long_option for extended string.
         * @param value_type if -x value or -long=yyy.
         * @param help string, future use.
         */
        Option(char short_option = 0, const char *long_option = NULL, const char *value_type = NULL, const char *help = NULL);

        virtual ~Option();

        static LinkedObject *first(void);

        /**
         * Disable a option.  Might happen if argv0 name suggests an
         * option is no longer actively needed.
         */
        void disable(void);

        /**
         * Used to send option into derived receiver.
         * @param value option that was received.
         * @return NULL or error string to use.
         */
        virtual const char *assign(const char *value) = 0;

        static void reset(void);
    };

    /**
     * Flag option for shell parsing.  This offers a quick-use class
     * to parse a shell flag, along with a counter for how many times
     * the flag was selected.  The counter might be used for -vvvv style
     * verbose options, for example.
     * @author David Sugar <dyfet@gnutelephony.org>
     */
    class __EXPORT flagopt : public Option
    {
    private:
        unsigned counter;
        bool single;

        virtual const char *assign(const char *value);

    public:
        flagopt(char short_option, const char *long_option = NULL, const char *help = NULL, bool single_use = true);

        inline operator bool()
            {return counter > 0;};

        inline bool operator!()
            {return counter == 0;};

        inline operator unsigned()
            {return counter;};

        inline unsigned operator*()
            {return counter;};

        inline void set(unsigned value = 1)
            {counter = value;};
    };

    /**
     * Grouping option.  This is used to create a grouping entry in
     * the shell::help() listing.
     * @author David Sugar <dyfet@gnutelephony.org>
     */
    class __EXPORT groupopt : public Option
    {
    private:
        virtual const char *assign(const char *value);

    public:
        groupopt(const char *help);
    };

    /**
     * Text option for shell parsing.  This offers a quick-use class
     * to parse a shell flag, along with a numeric text that may be
     * saved and a use counter, as multiple invocations is an error.
     * @author David Sugar <dyfet@gnutelephony.org>
     */
    class __EXPORT stringopt : public Option
    {
    private:
        bool used;

    protected:
        const char *text;

        virtual const char *assign(const char *value);

    public:
        stringopt(char short_option, const char *long_option = NULL, const char *help = NULL, const char *type = "text", const char *def_text = NULL);

        inline void set(const char *string)
            {text = string;};

        inline operator bool()
            {return used;};

        inline bool operator!()
            {return !used;};

        inline operator const char *()
            {return text;};

        inline const char *operator*()
            {return text;};
    };

    /**
     * Character option for shell parsing.  This offers a quick-use class
     * to parse a shell flag, along with a character code that may be
     * saved.  Multiple invocations is an error.
     * @author David Sugar <dyfet@gnutelephony.org>
     */
    class __EXPORT charopt : public Option
    {
    private:
        bool used;

    protected:
        char code;

        virtual const char *assign(const char *value);

    public:
        charopt(char short_option, const char *long_option = NULL, const char *help = NULL, const char *type = "char", char default_code = ' ');

        inline void set(char value)
            {code = value;};

        inline operator bool()
            {return used;};

        inline bool operator!()
            {return !used;};

        inline operator char()
            {return code;};

        inline char operator*()
            {return code;};
    };

    /**
     * Numeric option for shell parsing.  This offers a quick-use class
     * to parse a shell flag, along with a numeric value that may be
     * saved and a use counter, as multiple invocations is an error.
     * @author David Sugar <dyfet@gnutelephony.org>
     */
    class __EXPORT numericopt : public Option
    {
    private:
        bool used;

    protected:
        long number;

        virtual const char *assign(const char *value);

    public:
        numericopt(char short_option, const char *long_option = NULL, const char *help = NULL, const char *type = "numeric", long def_value = 0);

        inline void set(long value)
            {number = value;};

        inline operator bool()
            {return used;};

        inline bool operator!()
            {return !used;};

        inline operator long()
            {return number;};

        inline long operator*()
            {return number;};
    };

    /**
     * Counter option for shell parsing.  This offers a quick-use class
     * to parse a shell flag, along with a numeric value that may be
     * saved and a use counter, as multiple invocations is an error.  Unlike
     * numeric options, the short mode flag is a trigger option, and each
     * use of the short flag is considered a counter increment.
     * @author David Sugar <dyfet@gnutelephony.org>
     */
    class __EXPORT counteropt : public Option
    {
    private:
        bool used;

    protected:
        long number;

        virtual const char *assign(const char *value);

    public:
        counteropt(char short_option, const char *long_option = NULL, const char *help = NULL, const char *type = "numeric", long def_value = 0);

        inline void set(long value)
            {number = value;};

        inline operator bool()
            {return used;};

        inline bool operator!()
            {return !used;};

        inline operator long()
            {return number;};

        inline long operator*()
            {return number;};
    };

    /**
     * Construct a shell argument list by parsing a simple command string.
     * This separates a string into a list of command line arguments which
     * can be used with exec functions.
     * @param string to parse.
     * @param pagesize for local heap.
     */
    shell(const char *string, size_t pagesize = 0);

    /**
     * Construct a shell argument list from existing arguments.  This
     * copies and on some platforms expands the argument list originally
     * passed to main.
     * @param argc from main.
     * @param argv from main.
     * @param pagesize for local heap.
     */
    shell(int argc, char **argv, size_t pagesize = 0);

    /**
     * Construct an empty shell parser argument list.
     * @param pagesize for local heap.
     */
    shell(size_t pagesize = 0);

    static void setNumeric(numeric_t);

    static long getNumeric(void);

    /**
     * Display shell options.
     */
    static void help(void);

    /**
     * A shell system call.  This uses the native system shell to invoke the
     * command.
     * @param command string..
     * @param env array to optionally use.
     * @return error code of child process.
     */
    static int system(const char *command, const char **env = NULL);

    /**
     * A shell system call that can be issued using a formatted string.  This
     * uses the native system shell to invoke the command.
     * @param format of/command string.
     * @return error code of child process.
     */
    static int systemf(const char *format, ...) __PRINTF(1,2);

    /**
     * Set relative prefix.  Used for OS/X relocatable applications.
     * @param argv0 path of executable.
     */
    static void relocate(const char *argv0);

    /**
     * Get a system path.  This is used to get directories for application
     * specific data stores and default paths for config keys.
     * @param id of path to return.
     * @return path string or emptry string if not supported.
     */
    static String path(path_t id);

    /**
     * Get the system login id.
     * @return login id.
     */
    static String userid(void);

    /**
     * Get a merged path.  If the path requested is a full path, then
     * the prefix is ignored.
     * @param id of prefix.
     * @param directory path to merge with prefix.
     */
    static String path(path_t id, const char *directory);

    /**
     * Join a prefix with a path.
     * @param prefix to merge with.
     * @param directory or file path to merge.
     */
    static String path(String& prefix, const char *directory);

    /**
     * Bind application to text domain for internationalization.  This
     * is useful if the argv0 argument can vary because of a symlinked
     * executable.  This is the name of the .po/.mo message files for
     * your application.  If bind is not called before shell processing,
     * then the argv0 is used as the bind name.  Bind can be called
     * multiple times to change the default message catalog name of the
     * application, and this usage may be needed for plugins, though it's
     * generally recommended to use only once, and at the start of main().
     * @param name of text domain for the application.
     */
    static void bind(const char *name);

    /**
     * Rebind is used to change the text domain.  This may be needed in
     * applications which have separately built plugins that have thier
     * own message catalogs.  Normally the plugin would call bind itself
     * at initialization, and then use rebind to select either the
     * application's domain, or the plugins.  On systems without
     * internationalization, this has no effect.
     * @param name of text domain of plugin or NULL to restore application.
     */
    static void rebind(const char *name = NULL);

    /**
     * Parse a string as a series of arguments for use in exec calls.
     * @param string to parse.
     * @return argument array.
     */
    char **parse(const char *string);

    /**
     * Parse the command line arguments using the option table.  File
     * arguments will be expanded for wildcards on some platforms.
     * The argv will be set to the first file argument after all options
     * are parsed.
     * @param argc from main.
     * @param argv from main.
     */
    void parse(int argc, char **argv);

    /**
     * Get an environment variable.  This creates a local copy of the
     * variable in pager memory.
     * @param name of symbol.
     * @param value of symbol if not found.
     * @return value of symbol.
     */
    const char *getenv(const char *name, const char *value = NULL);

    /**
     * Get a local symbol.  This uses getenv if no local symbol is found.
     * @param name of symbol.
     * @param value of symbol if not found.
     * @return value of symbol.
     */
    const char *getsym(const char *name, const char *value = NULL);

    /**
     * Set a local symbol.
     * @param name of symbol to set.
     * @param value of symbol to set.
     */
    void setsym(const char *name, const char *value);

    /**
     * Test if symbol exists.
     * @param name of symbol.
     * @return true if found.
     */
    bool issym(const char *name);

    /**
     * Parse and extract the argv0 filename alone.
     * @param argv from main.
     * @return argv0 simple path name.
     */
    char *getargv0(char **argv);

    /**
     * Get the argument list by parsing options, and return the remaining
     * file arguments.  This is used by parse, and can be fed by main by
     * posting ++argv.
     * @param argv of first option.
     * @return argv of non-option file list.
     */
    char **getargv(char **argv);

    /**
     * Execute front-end like gdb based on stripped first argument.
     * @param argv0 of our executable.
     * @param argv to pass to child.
     * @param list of arguments to execute in front of argv.
     */
    void restart(char *argv0, char **argv, char **list);

    /**
     * Parse shell arguments directly into a shell object.
     * @param args table.
     * @param string to parse.
     * @return argument array.
     */
    inline static char **parse(shell &args, const char *string)
        {return args.parse(string);}

    /**
     * Get program name (argv0).
     */
    inline const char *argv0() const
        {return _argv0;}

    /**
     * Get the exec directory.
     */
    inline const char *execdir() const
        {return _exedir;}

    /**
     * Print error message and continue.
     * @param format string to use.
     */
    static void error(const char *format, ...) __PRINTF(1, 2);

    /**
     * Print error message and exit.  Ignored if exitcode == 0.
     * @param exitcode to return to parent process.
     * @param format string to use.
     */
    static void errexit(int exitcode, const char *format = NULL, ...) __PRINTF(2, 3);


    /**
     * Convert condition to exit code if true.
     * @param test condition.
     * @param exitcode to use if true.
     */
    static inline int condition(bool test, int exitcode)
        { return (test) ? exitcode : 0;};

    /**
     * Print a debug message by debug level.
     * @param level of debug message.
     * @param format string to use.
     */
    static void debug(unsigned level, const char *format, ...) __PRINTF(2, 3);

    /**
     * Print generic error message at specific error level.
     * @param level of error condition.
     * @param format string to use.
     */
    static void log(loglevel_t level, const char *format, ...) __PRINTF(2, 3);

    /**
     * Print security error message at specific error level.
     * @param level of error condition.
     * @param format string to use.
     */
    static void security(loglevel_t level, const char *format, ...) __PRINTF(2, 3);

    /**
     * Set logging level and state.
     * @param name of logging entity.
     * @param level of error conditions to log.
     * @param mode of logging.
     * @param handler for log messages.
     */
    static void log(const char *name, loglevel_t level = ERR, logmode_t mode = USER_LOG, logproc_t handler = (logproc_t)NULL);

    /**
     * Print to standard output.
     * @param format string to use.
     */
    static size_t printf(const char *format, ...) __PRINTF(1, 2);

    static size_t readln(char *address, size_t size);

    static size_t writes(const char *string);

    static size_t read(String& string);

    inline static size_t write(String& string)
        {return writes(string.c_str());};

    /**
     * Print to a pipe object.
     * @param pipe to write to.
     * @param format string to use.
     * @return number of bytes written.
     */
    static size_t printf(pipe_t pipe, const char *format, ...) __PRINTF(2, 3);

    /**
     * Read a line from a pipe object.
     * @param pipe to read from.
     * @param buffer to save into.
     * @param size of buffer.
     * @return number of bytes read.
     */
    static size_t readln(pipe_t pipe, char *buffer, size_t size);

    static size_t read(pipe_t pipe, String& string);

    static size_t writes(pipe_t pipe, const char *string);

    inline static size_t write(pipe_t pipe, String& string)
        {return writes(pipe, string.c_str());};

    /**
     * Get saved internal argc count for items.  This may be items that
     * remain after shell expansion and options have been parsed.
     * @return count of remaining arguments.
     */
    inline unsigned argc(void) const
        {return _argc;};

    /**
     * Get saved internal argv count for items in this shell object.  This
     * may be filename items only that remain after shell expansion and
     * options that have been parsed.
     * @return list of remaining arguments.
     */
    inline char **argv(void) const
        {return _argv;};

    /**
     * Return parser argv element.
     * @param offset into array.
     * @return argument string.
     */
    inline const char *operator[](unsigned offset)
        {return _argv[offset];};

    static void exiting(exitproc_t);

    /**
     * Detach current process to daemon for service entry.
     */
    void detach(mainproc_t mainentry = (mainproc_t)NULL);

    /**
     * Make current process restartable.
     */
    void restart(void);

    /**
     * Spawn a child process.  This creates a new child process.  If
     * the executable path is a pure filename, then the $PATH will be
     * used to find it.  The argv array may be created from a string
     * with the shell string parser.
     * @param path to executable.
     * @param argv list of command arguments for the child process.
     * @param env of child process can be explicitly set.
     * @param stdio handles for stdin, stdout, and stderr.
     * @return process id of child or INVALID_PID_VALUE if fails.
     */
    static shell::pid_t spawn(const char *path, char **argv, char **env = NULL, fd_t *stdio = NULL);

    /**
     * Spawn a child pipe.  If the executable path is a pure filename, then
     * the $PATH will be used to find it.  The argv array may be created from
     * a string with the shell string parser.
     * @param path to executable.
     * @param argv list of command arguments for the child process.
     * @param mode of pipe, rd only, wr only, or rdwr.
     * @param size of pipe buffer.
     * @param env of child process can be explicitly set.
     * @return pipe object or NULL on failure.
     */
    static shell::pipe_t spawn(const char *path, char **argv, pmode_t mode, size_t size = 512, char **env = NULL);

    /**
     * Set priority level and enable priority scheduler.  This activates the
     * realtime priority scheduler when a priority > 0 is requested for the
     * process, assuming scheduler support exists and the process is
     * sufficiently privileged.  Negative priorities are essentially the
     * same as nice.
     * @param pri level for process.
     */
    static void priority(int pri = 1);

    /**
     * Create a detached process.  This creates a new child process that
     * is completely detached from the current process.
     * @param path to executable.
     * @param argv list of command arguments for the child process.
     * @param env of child process can be explicitly set.
     * @param stdio handles for stdin, stdout, and stderr.
     * @return 0 if success, -1 on error.
     */
    static int  detach(const char *path, char **argv, char **env = NULL, fd_t *stdio = NULL);

    /**
     * Detach and release from parent process with exit code.
     * @param exit_code to send to parent process.
     */
    static void release(int exit_code = 0);

    /**
     * Wait for a child process to terminate.  This operation blocks.
     * @param pid of process to wait for.
     * @return exit code of process, -1 if fails or pid is invalid.
     */
    static int wait(shell::pid_t pid);

    /**
     * Cancel a child process.
     * @param pid of child process to cancel.
     * @return exit code of process, -1 if fails or pid is invalid.
     */
    static int cancel(shell::pid_t pid);

    /**
     * Wait for a child pipe to terminate.  This operation blocks.  If
     * the pipe io handle is dynamic, it is deleted.
     * @param pointer to pipe of child process to wait for.
     * @return exit code of process, -1 if fails or pid is invalid.
     */
    static int wait(shell::pipe_t pointer);

    /**
     * Cancel a child pipe.  If the pipe io handle is dynamic, it is deleted.
     * @param pointer to pipe of child process to cancel.
     * @return exit code of process, -1 if fails or pid is invalid.
     */
    static int cancel(shell::pipe_t pointer);

    /**
     * Return argc count.
     * @return argc count.
     */
    inline unsigned operator()(void)
        {return _argc;};

    /**
     * Text translation and localization.  This function does nothing but
     * return the original string if no internationalization support is
     * available.  If internationalization support exists, then it may
     * return a substituted translation based on the current locale.  This
     * offers a single function that can be safely used either when
     * internationalization support is present, or it is absent, eliminating
     * the need for the application to be coded differently based on
     * awareness of support.
     * @param string to translate.
     * @return translation if found or original text.
     */
    static const char *text(const char *string);

    /**
     * Plural text translation and localization.  This does nothing but
     * return single or plural forms if no internationalization is
     * enabled.  Else it uses ngettext().
     * @param singular string to translate.
     * @param plural string to translate.
     * @param count of objects.
     * @return string to use.
     */
    static const char *texts(const char *singular, const char *plural, unsigned long count);

    /**
     * Get argc count for an existing array.
     * @param argv to count items in.
     * @return argc count of array.
     */
    static unsigned count(char **argv);

#ifdef  _MSWINDOWS_

    static inline fd_t input(void)
        {return GetStdHandle(STD_INPUT_HANDLE);};

    static inline fd_t output(void)
        {return GetStdHandle(STD_OUTPUT_HANDLE);};

    static inline fd_t error(void)
        {return GetStdHandle(STD_ERROR_HANDLE);};

#else
    static inline fd_t input(void)
        {return 0;};

    static inline fd_t output(void)
        {return 1;};

    static inline fd_t error(void)
        {return 2;};
#endif

    static int inkey(const char *prompt = NULL);

    static char *getpass(const char *prompt, char *buffer, size_t size);

    static char *getline(const char *prompt, char *buffer, size_t size);

};

/**
 * Convenience type to manage and pass shell objects.
 */
typedef shell shell_t;

/**
 * Abusive compilers...
 */
#undef  _TEXT
#undef  _STR

/**
 * Invoke translation lookup if available.  This can also be used to
 * mark text constants that need to be translated.  It should not be
 * used with pointer variables, which should instead call shell::text
 * directly.  The primary purpose is to allow extraction of text to
 * be internationalized with xgettext "--keyword=_TEXT:1".
 */
inline  const char *_TEXT(const char *s)
    {return shell::text(s);}

inline const char *_STR(String& s)
    {return *s;}

END_NAMESPACE

#endif
