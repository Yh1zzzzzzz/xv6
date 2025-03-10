// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.
#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"
void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};
struct kmem_struct {
  struct spinlock lock;
  struct run *freelist;
};

struct kmem_struct kmem[NCPU];

uint64 mem_per_cpu;

void kinit() {
  mem_per_cpu = (uint64)PHYSTOP - (uint64)end;
  mem_per_cpu /= NCPU;
  // init perCPU lock
  for (int i = 0; i < NCPU; i++) {
    char name[10];
    snprintf(name, sizeof(name), "kmem%d", i);
    initlock(&kmem[i].lock, name);
  }
//  uint64 begin, stop;
  push_off();
//  int curr_cpu = cpuid();
  pop_off();
 // begin = (uint64)end + curr_cpu * mem_per_cpu;
  //stop = begin + mem_per_cpu - 1;
  //freerange((void *)begin, (void *)stop);
  freerange((void *)end, (void *)PHYSTOP);
  // split mem into 3 piece?
  //  freerange(end, (void *)PHYSTOP);
}

void freerange(void *pa_start, void *pa_end) {
  char *p;
  p = (char *)PGROUNDUP((uint64)pa_start);
  for (; p + PGSIZE <= (char *)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by pa,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void kfree(void *pa) {
  struct run *r;

  push_off();
  int CPUid = cpuid();
  pop_off();
  if (((uint64)pa % PGSIZE) != 0 || (char *)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run *)pa;

  acquire(&kmem[CPUid].lock);
  r->next = kmem[CPUid].freelist;
  kmem[CPUid].freelist = r;
  release(&kmem[CPUid].lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *kalloc(void) {
  struct run *r;

  push_off();
  int my_cpu = cpuid();
  pop_off();
  acquire(&kmem[my_cpu].lock);
  r = kmem[my_cpu].freelist;
  if (r)
    kmem[my_cpu].freelist = r->next;
  release(&kmem[my_cpu].lock);

  if (r)
    memset((char *)r, 5, PGSIZE); // fill with junk
  if (!r) {
    // steal from other cpu
    for (int id = 0; id < NCPU; id++) {
      acquire(&kmem[id].lock);
      r = kmem[id].freelist;
      if (r) {
        kmem[id].freelist = r->next;
        release(&kmem[id].lock);
        memset((char *)r, 5, PGSIZE);
        break;
      } else {
        release(&kmem[id].lock);
      }
    }
  }
  return (void *)r;
}
