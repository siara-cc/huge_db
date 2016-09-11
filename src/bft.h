#ifndef bft_H
#define bft_H
#include <cstdio>
#include <cstring>
#include <iostream>
#include "util.h"

using namespace std;

#define BFT_INT64MAP 1
#define BFT_9_BIT_PTR 0

#define BFT_NODE_SIZE 512

#if BFT_9_BIT_PTR == 1
#define MAX_PTR_BITMAP_BYTES 8
#define MAX_PTRS 63
#else
#define MAX_PTR_BITMAP_BYTES 0
#define MAX_PTRS 240
#endif
#define BFT_HDR_SIZE (MAX_PTR_BITMAP_BYTES+6)
#define IS_LEAF_BYTE buf[MAX_PTR_BITMAP_BYTES]
#define FILLED_SIZE buf + MAX_PTR_BITMAP_BYTES + 1
#define LAST_DATA_PTR buf + MAX_PTR_BITMAP_BYTES + 3
#define TRIE_LEN buf[MAX_PTR_BITMAP_BYTES+5]

#define INSERT_AFTER 1
#define INSERT_BEFORE 2
#define INSERT_LEAF 3
#define INSERT_EMPTY 4
#define INSERT_THREAD 5

#define MAX_KEY_PREFIX_LEN 20

class bft_iterator_status {
public:
    byte *t;
    int keyPos;
    byte is_next;
    byte is_child_pending;
    byte tp[MAX_KEY_PREFIX_LEN];
    bft_iterator_status(byte *trie) {
        t = trie + 1;
        keyPos = 0;
        is_child_pending = is_next = 0;
    }
};

class bft_node_handler {
private:
    static const byte x00 = 0;
    static const byte x01 = 1;
    static const byte x02 = 2;
    static const byte x03 = 3;
    static const byte x04 = 4;
    static const byte x05 = 5;
    static const byte x06 = 6;
    static const byte x07 = 7;
    static const byte x08 = 8;
    static const byte x80 = 0x80;
    static const byte xF8 = 0xF8;
    static const byte xFB = 0xFB;
    static const byte xFE = 0xFE;
    static const byte xFF = 0xFF;
    inline void insAt(byte *ptr, byte b);
    inline byte insAt(byte *ptr, byte b1, byte b2);
    inline byte insAt(byte *ptr, byte b1, byte b2, byte b3, byte b4);
    inline byte insAt(byte *ptr, byte b1, byte b2, byte b3, byte b4, byte b5);
    inline void setAt(byte pos, byte b);
    inline void append(byte b);
    inline void appendPtr(int16_t p);
    inline byte getAt(byte pos);
    inline void delAt(byte *ptr);
    inline void delAt(byte *ptr, int16_t count);
    inline void insBit(uint32_t *ui32, int pos, int16_t kv_pos);
    inline void insBit(uint64_t *ui64, int pos, int16_t kv_pos);
    byte *nextPtr(bft_iterator_status& s);
    //byte *nextPtr(bft_iterator_status& s,
    //        bft_node_handler *other_trie, bft_iterator_status *s_last);
    int16_t getLastPtrOfChild(byte *triePos);
    inline byte *getLastPtr(byte *last_t);
    static byte *alignedAlloc();
public:
    byte *buf;
    byte *trie;
    byte *triePos;
    byte *origPos;
    int16_t need_count;
    byte insertState;
    byte isPut;
    int keyPos;
    const char *key;
    int key_len;
    const char *key_at;
    int16_t key_at_len;
    int16_t last_child_pos;
    const char *value;
    int16_t value_len;
    const byte *keyFoundAt;
    bft_node_handler(byte *m);
    void initBuf();
    inline void initVars();
    void setBuf(byte *m);
    bool isFull(int16_t kv_lens);
    inline bool isLeaf();
    inline void setLeaf(char isLeaf);
    int16_t filledSize();
    inline void setFilledSize(int16_t filledSize);
    inline int16_t getKVLastPos();
    inline void setKVLastPos(int16_t val);
    void addData(int16_t pos);
    byte *getChild(int16_t pos);
    inline byte *getKey(int16_t ptr, int16_t *plen);
    byte *getData(int16_t ptr, int16_t *plen);
    byte *split(int16_t *pbrk_idx, char *first_key, int16_t *first_len_ptr);
    int16_t locateKeyInLeaf();
    void traverseToLeaf(byte *node_paths[]);
    byte *getFirstPtr();
    int16_t insertCurrent();
    void updatePtrs(byte *upto, int diff);
};

class bft {
private:
    long total_size;
    int numLevels;
    int maxKeyCount;
    int blockCount;
    void recursiveUpdate(bft_node_handler *node, int16_t pos,
            byte *node_paths[], int16_t level);
public:
    byte *root_data;
    int maxThread;
    static int count1, count2;
    static byte split_buf[BFT_NODE_SIZE];
    bft();
    ~bft();
    char *get(const char *key, int16_t key_len, int16_t *pValueLen);
    void put(const char *key, int16_t key_len, const char *value,
            int16_t value_len);
    void printMaxKeyCount(long num_entries) {
        std::cout << "Block Count:" << blockCount << std::endl;
        std::cout << "Avg Block Count:" << (num_entries / blockCount)
                << std::endl;
        std::cout << "Avg Max Count:" << (maxKeyCount / blockCount)
                << std::endl;
    }
    void printNumLevels() {
        std::cout << "Level Count:" << numLevels << std::endl;
        std::cout << "Count1:" << count1 << std::endl;
        std::cout << "Count2:" << count2 << std::endl;
    }
    long size() {
        return total_size;
    }
};

#endif