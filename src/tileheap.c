#include "tileheap.h"
#include <stdlib.h>
#include <float.h>

// TODO: Ce truc de "size", c'est nul, il faut changer et initialiser avec que des éléments DBL_MAX
// Et ne faire que des percolations bas à l'ajout

// ---------- utils functions -------------
static int heap_elem_compare(const void *he1, const void *he2) {          // function used to compare two heap_elements in qsort
  // returns <0 if he1 < he2
  // returns =0 if he1 == he2
  // returns >0 if he1 > he2
  double s1 = ((heap_elem *)he1)->match_score;
  double s2 = ((heap_elem *)he2)->match_score;
  return (s2 < s1) - (s1 < s2);
}

static void heap_precole_upward(tile_heap *h, unsigned int idx) {     // percole an element up in the heap
  if (idx > 0) {
    unsigned int parent = (idx-1)/2;
    if (h->data[parent].match_score < h->data[idx].match_score) {
      heap_elem tmp = h->data[parent];
      h->data[parent] = h->data[idx];
      h->data[idx] = tmp;
      heap_precole_upward(h, parent);
    }
  }
}

static void heap_percole_downward(tile_heap *h, unsigned int idx) {    // percole an element down in the heap
  if (2*idx + 1 >= h->size) {           // if we are outside the boundaries, stop
    return;
  }
  unsigned int largest = idx;
  unsigned int l = 2*idx + 1;
  unsigned int r = 2*idx + 2;
  if (l < h->size && h->data[l].match_score > h->data[largest].match_score) {
    largest = l;      // is left the largest ?
  }
  if (r < h->size && h->data[r].match_score > h->data[largest].match_score) {
    largest = r;      // is right the largest ?
  }
  if (largest == idx) {   // if no children is larger, just stop
    return;
  }
  heap_elem tmp = h->data[idx];
  h->data[idx] = h->data[largest];
  h->data[largest] = tmp;
  heap_percole_downward(h, largest);
}
// ----------------------------------------

static void th_init(tile_heap *h, unsigned int capacity) {
  h->data = calloc(capacity, sizeof(heap_elem));
  h->size = 0;
  h->capacity = capacity;
}

static double th_get_max_score(tile_heap *h) {
  if (h->size < h->capacity) {
    return DBL_MAX;
  }
  return h->data[0].match_score;
}

static void th_push(tile_heap *h, heap_elem elem) {
  if (h->size < h->capacity) {
    h->data[h->size] = elem;
    heap_precole_upward(h, h->size++);
  } else if (elem.match_score < h->data[0].match_score) {
    h->data[0] = elem;
    heap_percole_downward(h, 0);
  }
}

static heap_elem *th_get_sorted_elems(tile_heap *h) {
  qsort(h->data, h->size, sizeof(heap_elem), heap_elem_compare);
  return h->data;
}
