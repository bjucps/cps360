#ifndef VFS_XV6_H
#define VFS_XV6_H

extern struct vfs_ops xv6fs_vfs_ops;

struct vfile* xv6fs_create(char *, short, short, short);
void xv6fs_stati(struct vfile *, struct stat *st);
void xv6fs_ilock(struct vfile*);
void xv6fs_iunlock(struct vfile*);
void xv6fs_iput(struct vfile*);
int xv6fs_writei(struct vfile*, char *src, uint off, uint n);
int xv6fs_readi(struct vfile*, char *src, uint off, uint n);

#endif