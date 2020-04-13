#ifndef VFS_H
#define VFS_H

struct vfs_ops;

struct vfile {
  int refcount;
  void *fsp; // filesystem-specific file pointer (ex. struct inode*)
  int type; // T_DIR, etc. (See stat.h)
  struct vfs_ops *vops;
};


struct vfs_ops {
  void (*stati)(struct vfile *, struct stat *st);
  void (*ilock)(struct vfile*);
  void (*iunlock)(struct vfile*);
  void (*iput)(struct vfile*);
  int (*writei)(struct vfile*, char *src, uint off, uint n);
  int (*readi)(struct vfile*, char *src, uint off, uint n);
};

void vfsinit();

struct vfile *
vfile_alloc(void *data, struct vfs_ops* vo);

struct vfile*
vfs_create(char *path, short type, short major, short minor);

int
vfs_write(struct vfile *vfile, char *src, uint off, uint n);

int
vfs_read(struct vfile *vfile, char *dst, uint off, uint n);

void
vfs_stat(struct vfile *vfile, struct stat *);

struct vfile*
vfs_namei(char *path);

void
vfs_ilock(struct vfile*);

void
vfs_iput(struct vfile*);

void
vfs_iunlockput(struct vfile*);

void
vfs_iunlock(struct vfile*);

#endif