#ifndef BUF_H_
#define BUF_H_

// FS specific
#define BSIZE 512  // block size

struct buf {
  int valid;   // has data been read from disk?
  int disk;    // does disk "own" buf?
  uint dev;
  uint blockno;
  /* struct sleeplock lock; */
  uint refcnt;
  struct buf *prev; // LRU cache list
  struct buf *next;
  uchar data[BSIZE];
};

#endif // BUF_H_
