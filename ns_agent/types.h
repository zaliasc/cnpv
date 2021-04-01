#ifndef TYPES_H
#define TYPES_H

#include <pthread.h>
#include <sys/types.h>

/* open/fcntl.  */
#define O_ACCMODE	   0003
#define O_RDONLY	     00
#define O_WRONLY	     01
#define O_RDWR		     02
#ifndef O_CREAT
# define O_CREAT	   0100	/* Not fcntl.  */
#endif
#ifndef O_EXCL
# define O_EXCL		   0200	/* Not fcntl.  */
#endif
#ifndef O_NOCTTY
# define O_NOCTTY	   0400	/* Not fcntl.  */
#endif
#ifndef O_TRUNC
# define O_TRUNC	  01000	/* Not fcntl.  */
#endif
#ifndef O_APPEND
# define O_APPEND	  02000
#endif
#ifndef O_NONBLOCK
# define O_NONBLOCK	  04000
#endif
#ifndef O_NDELAY
# define O_NDELAY	O_NONBLOCK
#endif
#ifndef O_SYNC
# define O_SYNC	       04010000
#endif
#define O_FSYNC		O_SYNC
#ifndef O_ASYNC
# define O_ASYNC	 020000
#endif
#ifndef __O_LARGEFILE
# define __O_LARGEFILE	0100000
#endif

/* The constants AT_REMOVEDIR and AT_EACCESS have the same value.  AT_EACCESS
   is meaningful only to faccessat, while AT_REMOVEDIR is meaningful only to
   unlinkat.  The two functions do completely different things and therefore,
   the flags can be allowed to overlap.  For example, passing AT_REMOVEDIR to
   faccessat would be undefined behavior and thus treating it equivalent to
   AT_EACCESS is valid undefined behavior.  */
#ifdef __USE_ATFILE
# define AT_FDCWD		-100	/* Special value used to indicate
					   the *at functions should use the
					   current working directory. */
# define AT_SYMLINK_NOFOLLOW	0x100	/* Do not follow symbolic links.  */
# define AT_REMOVEDIR		0x200	/* Remove directory instead of
					   unlinking file.  */
# define AT_SYMLINK_FOLLOW	0x400	/* Follow symbolic links.  */
# ifdef __USE_GNU
#  define AT_NO_AUTOMOUNT	0x800	/* Suppress terminal automount
					   traversal.  */
#  define AT_EMPTY_PATH		0x1000	/* Allow empty relative pathname.  */
#  define AT_STATX_SYNC_TYPE	0x6000
#  define AT_STATX_SYNC_AS_STAT	0x0000
#  define AT_STATX_FORCE_SYNC	0x2000
#  define AT_STATX_DONT_SYNC	0x4000
#  define AT_RECURSIVE		0x8000	/* Apply to the entire subtree.  */
# endif
# define AT_EACCESS		0x200	/* Test access permitted for
					   effective IDs, not real IDs.  */
#endif

/* my define. */
#define O_READ 0100
#define O_WRITE 0010
// #define O_EXEC 0001

#define MAX_PATH 512

struct real_calls
{
  int (*real_open)(const char *pathname, int flags, ...);
  int (*real_openat)(int dirfd, const char *pathname, int flags, ...); 
  int (*real_fopen)(const char *pathname, const char *mode);
  int (*real_dprintf)(int fd, const char *format, ...);
};

typedef enum {
  OPEN,
  OPENAT,
  FOPEN
} FUNC_TYPE ;

struct handle_args {
        FUNC_TYPE type;
        // open
        char * pathname;
        int flags;
        mode_t mode;
        // openat
        int dirfd;
        // fopen
        const char * f_mode;
};

struct user {
  char pathname[MAX_PATH];
  int permission;
};


#endif /* TYPES_H */
