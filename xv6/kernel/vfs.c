#include "types.h"
#include "defs.h"
#include "spinlock.h"
#include "fs.h"
#include "vfs.h"
#include "file.h"
#include "stat.h"
#include "vfs_xv6fs.h"

#define MAX_VFILES 100

struct {
  struct spinlock lock;
  struct vfile vfiles[MAX_VFILES];
} vfs;

void
vfsinit(void)
{
  initlock(&vfs.lock, "vfs");
}

struct vfile *
vfile_lookup(void *fsp)
{
  struct vfile *vf = NULL;
  acquire(&vfs.lock);
  for (int i = 0; i < MAX_VFILES; ++i) {
    if (vfs.vfiles[i].fsp == fsp) {
      vf = &vfs.vfiles[i];
      vf->refcount++;
      break;
    }
  }
  release(&vfs.lock);
  return vf;
}

struct vfile *
vfile_alloc(void *fsp, struct vfs_ops* vo)
{
  if (!fsp) {
    panic("vfile_alloc: fsp is NULL");
  }

  if (!vo) {
    panic("vfile_alloc: vo is NULL");
  }

  acquire(&vfs.lock);
  for (int i = 0; i < MAX_VFILES; ++i) {
    if (vfs.vfiles[i].refcount == 0) {
      vfs.vfiles[i].fsp = fsp;
      vfs.vfiles[i].vops = vo;
      vfs.vfiles[i].refcount = 1;
      release(&vfs.lock);
      // cprintf("Allocated vfile #%d\n", i);
      return &vfs.vfiles[i];
    }
  }
  release(&vfs.lock);
  panic("vfile_alloc: no vfile entries available");
}

struct vfile*
vfs_create(char *path, short type, short major, short minor)
{
  return xv6fs_create(path, type, major, minor);
}

int
vfs_write(struct vfile *vfile, char *src, uint off, uint n)
{
  return vfile->vops->writei(vfile, src, off, n);
}

int
vfs_read(struct vfile *vfile, char *dst, uint off, uint n)
{
  return vfile->vops->readi(vfile, dst, off, n);
}

void
vfs_stat(struct vfile *vfile, struct stat *stat)
{
  vfile->vops->stati(vfile, stat);
}


struct vfile*
vfs_namei(char *path)
{
  struct inode *ip = namei(path);
  if (!ip)
    return NULL;  

  struct vfile* vf = vfile_lookup(ip);

  if (!vf) 
    vf = vfile_alloc(ip, &xv6fs_vfs_ops);
  
  return vf;
}

void
vfs_ilock(struct vfile* vfile)
{
  vfile->vops->ilock(vfile);
}


void
vfs_iput(struct vfile* vfile)
{
  if (vfile->refcount == 0) {
    panic("vfs_iput: refcount == 0");
  }
  vfile->vops->iput(vfile);
  vfile->refcount--;
}

void
vfs_iunlockput(struct vfile* vfile)
{
  vfs_iunlock(vfile);
  vfs_iput(vfile);
}

void
vfs_iunlock(struct vfile* vfile)
{
  vfile->vops->iunlock(vfile);
}