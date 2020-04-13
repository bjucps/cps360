// Simple PIO-based (non-DMA) IDE driver code.

#include "types.h"
#include "defs.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"
#include "traps.h"
#include "spinlock.h"
#include "buf.h"

#define IDE_BSY       0x80
#define IDE_DRDY      0x40
#define IDE_DF        0x20
#define IDE_ERR       0x01

#define IDE_CMD_READ  0x20
#define IDE_CMD_WRITE 0x30

// idequeue points to the buf now being read/written to the disk.
// idequeue->qnext points to the next buf to be processed.
// You must hold idelock while manipulating queue.

static struct spinlock idelock;
static struct buf *idequeue;

static int havedisk1;
static int havedisk2;
static void idestart(struct buf*);


#define MASTER_CONTROL_PORT 0x3F6
#define SLAVE_CONTROL_PORT 0x376
#define MASTER_BASE_PORT 0x1F0
#define SLAVE_BASE_PORT 0x170

static ushort
ide_control_port(uint dev)
{
  return (dev < 2) ? MASTER_CONTROL_PORT : SLAVE_CONTROL_PORT;
}

static ushort
ide_base_port(uint dev)
{
  return (dev < 2) ? MASTER_BASE_PORT : SLAVE_BASE_PORT;
}

// Wait for IDE disk to become ready.
static int
idewait(uint dev, int checkerr)
{
  int r;
  ushort port = ide_base_port(dev) + 7;

  // cprintf("Waiting for device %d\n", dev);

  while(((r = inb(port)) & (IDE_BSY|IDE_DRDY)) != IDE_DRDY) 
    ;
  if(checkerr && (r & (IDE_DF|IDE_ERR)) != 0)
    return -1;

  // cprintf("Wait for device %d complete\n", dev);

  return 0;
}

void
ideinit(void)
{
  int i;

  initlock(&idelock, "ide");
  picenable(IRQ_IDE);
  picenable(IRQ_IDE+1);
  ioapicenable(IRQ_IDE, ncpu - 1);
  ioapicenable(IRQ_IDE+1, ncpu - 1);
  idewait(0, 0);
  
  // Check if disk 1 is present
  outb(0x1f6, 0xe0 | (1<<4));
  for(i=0; i<1000; i++){
    if(inb(0x1f7) != 0){
      havedisk1 = 1;
      break;
    }
  }
  
  // Switch back to disk 0.
  outb(0x1f6, 0xe0 | (0<<4));

  // Check if disk 2 is present
  outb(0x176, 0xe0);
  for(i=0; i<1000; i++){
    if(inb(0x177) != 0){
      havedisk2 = 1;
      cprintf("Have disk 2!\n");
      break;
    }
  }

}

// Start the request for b.  Caller must hold idelock.
static void
idestart(struct buf *b)
{
  if(b == 0)
    panic("idestart");

  ushort control_port = ide_control_port(b->dev);
  ushort base_port = ide_base_port(b->dev);

  idewait(b->dev, 0);
  outb(control_port, 0);  // generate interrupt
  outb(base_port + 2, 1);  // number of sectors
  outb(base_port + 3, b->sector & 0xff);
  outb(base_port + 4, (b->sector >> 8) & 0xff);
  outb(base_port + 5, (b->sector >> 16) & 0xff);
  outb(base_port + 6, 0xe0 | ((b->dev&1)<<4) | ((b->sector>>24)&0x0f));
  if(b->flags & B_DIRTY){
    outb(base_port + 7, IDE_CMD_WRITE);
    outsl(base_port, b->data, 512/4);
  } else {
    outb(base_port + 7, IDE_CMD_READ);
  }
}

// Interrupt handler.
void
ideintr(void)
{
  struct buf *b;

  // Take first buffer off queue.
  acquire(&idelock);
  if((b = idequeue) == 0){
    release(&idelock);
    // cprintf("spurious IDE interrupt\n");
    return;
  }
  idequeue = b->qnext;

  // Read data if needed.
  if(!(b->flags & B_DIRTY) && idewait(b->dev, 1) >= 0)
    insl(ide_base_port(b->dev), b->data, 512/4);
  
  // Wake process waiting for this buf.
  b->flags |= B_VALID;
  b->flags &= ~B_DIRTY;
  wakeup(b);
  
  // Start disk on next buf in queue.
  if(idequeue != 0)
    idestart(idequeue);

  release(&idelock);
}

// Sync buf with disk. 
// If B_DIRTY is set, write buf to disk, clear B_DIRTY, set B_VALID.
// Else if B_VALID is not set, read buf from disk, set B_VALID.
void
iderw(struct buf *b)
{
  struct buf **pp;

  if(!(b->flags & B_BUSY))
    panic("iderw: buf not busy");
  if((b->flags & (B_VALID|B_DIRTY)) == B_VALID)
    panic("iderw: nothing to do");
  if(b->dev == 1 && !havedisk1)
    panic("iderw: ide disk 1 not present");
  if(b->dev == 2 && !havedisk2)
    panic("iderw: ide disk 2 not present");

  acquire(&idelock);

  // Append b to idequeue.
  b->qnext = 0;
  for(pp=&idequeue; *pp; pp=&(*pp)->qnext)
    ;
  *pp = b;
  
  // Start disk if necessary.
  if(idequeue == b)
    idestart(b);
  
  // Wait for request to finish.
  // Assuming will not sleep too long: ignore proc->killed.
  while((b->flags & (B_VALID|B_DIRTY)) != B_VALID){
    sleep(b, &idelock);
  }

  release(&idelock);
}
