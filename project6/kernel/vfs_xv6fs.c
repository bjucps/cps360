#include "types.h"
#include "defs.h"
#include "spinlock.h"
#include "fs.h"
#include "vfs.h"
#include "file.h"
#include "stat.h"
#include "vfs_xv6fs.h"

struct vfs_ops xv6fs_vfs_ops = {
  .stati = xv6fs_stati,
  .ilock = xv6fs_ilock,
  .iunlock = xv6fs_iunlock,
  .iput = xv6fs_iput,
  .writei = xv6fs_writei,
  .readi = xv6fs_readi
};

struct inode*
create(char *path, short type, short major, short minor);


struct vfile* xv6fs_create(char *path, short type, short major, short minor)
{
  struct inode *ip = create(path, type, major, minor);
  if (!ip)
    return NULL;

  return vfile_alloc(ip, &xv6fs_vfs_ops);

}

void xv6fs_stati(struct vfile *vfile, struct stat *st)
{
  stati(vfile->fsp, st);
}

void xv6fs_ilock(struct vfile* vfile)
{
  ilock(vfile->fsp);
  vfile->type = ((struct inode *)vfile->fsp)->type;
}

void xv6fs_iunlock(struct vfile* vfile)
{
  iunlock(vfile->fsp);
}

void xv6fs_iput(struct vfile* vfile) 
{
  iput(vfile->fsp);
}

int xv6fs_writei(struct vfile* vfile, char *src, uint off, uint n)
{
  return writei(vfile->fsp, src, off, n);
}

int xv6fs_readi(struct vfile* vfile, char *src, uint off, uint n)
{
  return readi(vfile->fsp, src, off, n);
}


