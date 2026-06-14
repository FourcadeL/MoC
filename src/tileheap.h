// A heap implementatio to find the best tile matches
// Follow a max heap implementation


#ifndef TILE_HEAP_H
#define TILE_HEAP_H

typedef struct {
  double match_score;           // the element match score (lesser is better)
  unsigned int tile_index;      // the tile index of the score
} heap_elem;

typedef struct {
  heap_elem *data;              // the content of the heap
  unsigned int size;            // the current size of the heap
  unsigned int capacity;        // the maximum capacity of the heap
} tile_heap;

// ------------ Functions defs ------------
void th_init(tile_heap *h, unsigned int capacity);     // initialize a heap in place
double th_get_max_score(tile_heap *h);                 // return the maximum score of the heap
void th_push(tile_heap *h, heap_elem elem);            // pushes a new heap_element onto the stack
// WARNING: This destroys the heap structure
heap_elem *th_get_sorted_elems(tile_heap *h);          // return the sorted elements of the heap
// ----------------------------------------

#endif // !TILE_HEAP_H
