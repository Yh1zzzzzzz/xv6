// Buffer cache.
//
// The buffer cache is a linked list of buf structures holding
// cached copies of disk block contents.  Caching disk blocks
// in memory reduces the number of disk reads and also provides
// a synchronization point for disk blocks used by multiple processes.
//
// Interface:
// * To get a buffer for a particular disk block, call bread.
// * After changing buffer data, call bwrite to write it to disk.
// * When done with the buffer, call brelse.
// * Do not use the buffer after calling brelse.
// * Only one process at a time can use a buffer,
//     so do not keep them longer than necessary.

#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "riscv.h"
#include "defs.h"
#include "fs.h"
#include "buf.h"
#define num_Backet 13 
struct {
  struct spinlock lock;
  struct buf buf[NBUF];

  // Linked list of all buffers, through prev/next.
  // Sorted by how recently the buffer was used.
  // head.next is most recent, head.prev is least.
  struct buf head;
} bcache;
struct hash_table
{
  struct buf head;
  int num;
};
uint tickss;
struct spinlock hash_lock[num_Backet];
struct hash_table Hash_table[num_Backet];



void binit(void) {
  struct buf *b;

  initlock(&bcache.lock, "bcache");
  for (int i = 0; i < num_Backet; i++) {
    char name[10];
    snprintf(name, sizeof(name), "back%d", i);
    initlock(&hash_lock[i], name);
    Hash_table[i].num = 0;
    Hash_table[i].head.next = &Hash_table[i].head;
    Hash_table[i].head.prev = &Hash_table[i].head;
  }
  // Create linked list of buffers
  for (b = bcache.buf; b < bcache.buf + NBUF; b++) {
    initsleeplock(&b->lock, "buffer");
    b->ticks = 0;
  }
  tickss = 0;
}

// Look through buffer cache for block on device dev.
// If not found, allocate a buffer.
// In either case, return locked buffer.
static struct buf *bget(uint dev, uint blockno) {
  struct buf *b;
  int hash = blockno % num_Backet;

  acquire(&hash_lock[hash]);
  
  // Is the block already cached?
  for (b = Hash_table[hash].head.next; b != &Hash_table[hash].head; b = b->next) {
    if (b->dev == dev && b->blockno == blockno) {
      b->refcnt++;
      release(&hash_lock[hash]);
      acquiresleep(&b->lock);
      return b;
    }
  }

  // Not cached.
  acquire(&bcache.lock);
  // Recycle the least recently used (LRU) unused buffer.
  uint min_tick = 0xffffffff;
  struct buf *min_b = 0;
  for (b = bcache.buf; b < bcache.buf + NBUF; b++) {
    if (b->refcnt == 0) {
      // 如果缓冲区已经在某个哈希链表中，需要先移除
      
      // b->dev = dev;
      // b->blockno = blockno;
      // b->valid = 0;
      // b->refcnt = 1;
      
      // // 添加到新的哈希链表头部
      // b->next = Hash_table[hash].head.next;
      // b->prev = &Hash_table[hash].head;
      // Hash_table[hash].head.next->prev = b;
      // Hash_table[hash].head.next = b;
      if (b->ticks < min_tick) {
        min_tick = b->ticks;
        min_b = b;
      }
        
    }
  }
  min_b->dev = dev;
  min_b->blockno = blockno;
  min_b->valid = 0;
  min_b->refcnt = 1;
  // 添加到新的哈希链表头部
  min_b->next = Hash_table[hash].head.next;
  min_b->prev = &Hash_table[hash].head;
  Hash_table[hash].head.next->prev = min_b;
  Hash_table[hash].head.next = min_b;
  
  release(&bcache.lock);
  release(&hash_lock[hash]);
  if(min_b){
    acquiresleep(&min_b->lock);
    return min_b;
  }
  panic("bget: no buffers");
}

// Return a locked buf with the contents of the indicated block.
struct buf *bread(uint dev, uint blockno) {
  struct buf *b;

  b = bget(dev, blockno);
  if (!b->valid) {
    virtio_disk_rw(b, 0);
    b->valid = 1;
  }
  return b;
}

// Write b's contents to disk.  Must be locked.
void bwrite(struct buf *b) {
  if (!holdingsleep(&b->lock))
    panic("bwrite");
  virtio_disk_rw(b, 1);
}

// Release a locked buffer.
// Move to the head of the most-recently-used list.
void brelse(struct buf *b) {
  if (!holdingsleep(&b->lock))
    panic("brelse");

  releasesleep(&b->lock);
  
  int hash = b->blockno % num_Backet;
  acquire(&hash_lock[hash]);
  b->refcnt--;
  
  if (b->refcnt == 0) {
    // no one is waiting for it.
    // 将缓冲区保持在哈希链表中以便重用
    // 可选：移动到链表头部表示最近使用过
    b->next->prev = b->prev;
    b->prev->next = b->next;
    b->prev = 0;
    b->next = 0;
    tickss++;
    b->ticks=tickss;
  }
  
  release(&hash_lock[hash]);
}

void bpin(struct buf *b) {
  int hash = b->blockno % num_Backet;
  acquire(&hash_lock[hash]);
  b->refcnt++;
  release(&hash_lock[hash]);
}


void bunpin(struct buf *b) {
  int hash = b->blockno % num_Backet;
  acquire(&hash_lock[hash]);
  b->refcnt--;
  release(&hash_lock[hash]);
}
