#include "GenTree.h"

byte GenTree::bit_count[256];
byte GenTree::last_bit_mask[256];
int16_t *GenTree::roots;
int16_t *GenTree::left;
int16_t *GenTree::ryte;
int16_t *GenTree::parent;
int16_t GenTree::ixRoots;
int16_t GenTree::ixLeft;
int16_t GenTree::ixRyte;
int16_t GenTree::ixPrnt;
uint32_t GenTree::left_mask32[32];
uint32_t GenTree::ryte_mask32[32];
uint32_t GenTree::mask32[32];
