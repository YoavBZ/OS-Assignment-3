#include "types.h"
#include "stat.h"
#include "user.h"
#include "param.h"

// Memory allocator by Kernighan and Ritchie,
// The C programming Language, 2nd ed.  Section 8.7.

typedef long Align;

#define PGSIZE          4096    // bytes mapped by a page
#define PGROUNDUP(sz)  (((sz)+PGSIZE-1) & ~(PGSIZE-1))
#define PTE_W           0x002   // Writeable
#define PTE_PMLC        0x400   // Page allocated by pmalloc


union header {
  struct {
    union header *ptr;
    uint size;
  } s;
  Align x;
};

typedef union header Header;

static Header base;
static Header *freep;

void
free(void *ap)
{
  Header *bp, *p;

  bp = (Header*)ap - 1;
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
      break;
  if(bp + bp->s.size == p->s.ptr){
    bp->s.size += p->s.ptr->s.size;
    bp->s.ptr = p->s.ptr->s.ptr;
  } else
    bp->s.ptr = p->s.ptr;
  if(p + p->s.size == bp){
    p->s.size += bp->s.size;
    p->s.ptr = bp->s.ptr;
  } else
    p->s.ptr = bp;
  freep = p;
}

static Header*
morecore(uint nu)
{
  char *p;
  Header *hp;

  if(nu < 4096)
    nu = 4096;
  p = sbrk(nu * sizeof(Header));
  if(p == (char*)-1)
    return 0;
  hp = (Header*)p;
  hp->s.size = nu;
  free((void*)(hp + 1));
  return freep;
}

void*
malloc(uint nbytes)
{
  Header *p, *prevp;
  uint nunits;

  nunits = (nbytes + sizeof(Header) - 1)/sizeof(Header) + 1;
  if((prevp = freep) == 0){
    base.s.ptr = freep = prevp = &base;
    base.s.size = 0;
  }
  for(p = prevp->s.ptr; ; prevp = p, p = p->s.ptr){
    if(p->s.size >= nunits){
      if(p->s.size == nunits)
        prevp->s.ptr = p->s.ptr;
      else {
        p->s.size -= nunits;
        p += p->s.size;
        p->s.size = nunits;
      }
      freep = prevp;
      return (void*)(p + 1);
    }
    if(p == freep)
      if((p = morecore(nunits)) == 0)
        return 0;
  }
}

int isAligned(Header *p) {
  return (int) p % PGSIZE == 0;
}

void *
pmalloc() {
  Header *p, *prevp;
  uint nunits;

  nunits = (PGSIZE + sizeof(Header)) / sizeof(Header);
  if ((prevp = freep) == 0) {
    base.s.ptr = freep = prevp = &base;
    base.s.size = 0;
  }
  for (p = prevp->s.ptr;; prevp = p, p = p->s.ptr) {
    if (p->s.size == nunits && isAligned(p+1)){
      prevp->s.ptr = p->s.ptr;
      freep = prevp;
      setflag((char *) (p + 1), PTE_PMLC, 1);
      return (void *) (p + 1);
    }
    if (p->s.size > nunits && (((Header *)PGROUNDUP((uint)(p+1)) + PGSIZE <= p + p->s.size) || isAligned(p+1))){
      // in the end of the block
      if (isAligned(p->s.size + p - PGSIZE)) {
        p->s.size -= nunits;
        p += p->s.size;
        p->s.size = nunits;
      } else if (isAligned(p+1)) {
        Header *tempp = p + nunits;
        prevp->s.ptr = tempp;
        tempp->s.ptr = p->s.ptr;
        tempp->s.size = p->s.size - nunits;
        p->s.size = nunits;
      } else {
        Header *tempp = (Header *)PGROUNDUP((uint)(p+1)) - 1;
        Header *tempp2 = tempp + nunits;
        tempp2->s.ptr = p->s.ptr;
        p->s.ptr = tempp2;
        tempp2->s.size = p + p->s.size - tempp2;
        p->s.size -= (tempp2->s.size + nunits);
        p = tempp;
        p->s.size = nunits;
      }
      freep = prevp;
      setflag((char *) (p + 1), PTE_PMLC, 1);
      return (void *) (p + 1);
    }
    if (p == freep)
      if ((p = morecore(nunits)) == 0)
        return 0;
  }
}

int
pfree(void *ap) {
  if (!flags((char *) ap, PTE_W)) {
    return -1;
  }
  setflag(ap, PTE_W, 1);
  setflag(ap, PTE_PMLC, 0);
  free(ap);
  return 1;
}

int
protect_page(void *ap) {
  if (isAligned(ap) && flags(ap, PTE_PMLC)) {
    return setflag(ap, PTE_W, 0);
  }
  return -1;
}