#ifndef dfox_H
#define dfox_H
#include <cstdio>
#include <cstring>
#include <iostream>
#include "util.h"

using namespace std;

#define DX_INT64MAP 1

#define DFOX_NODE_SIZE 512

#if DFOX_NODE_SIZE == 512
#define MAX_PTR_BITMAP_BYTES 8
#define MAX_PTRS 63
#else
#define MAX_PTR_BITMAP_BYTES 0
#define MAX_PTRS 240
#endif
#define DFOX_HDR_SIZE (MAX_PTR_BITMAP_BYTES+6)
#define IS_LEAF_BYTE buf[MAX_PTR_BITMAP_BYTES]
#define FILLED_SIZE buf + MAX_PTR_BITMAP_BYTES + 1
#define LAST_DATA_PTR buf + MAX_PTR_BITMAP_BYTES + 3
#define TRIE_LEN buf[MAX_PTR_BITMAP_BYTES+5]

#define INSERT_MIDDLE1 1
#define INSERT_MIDDLE2 2
#define INSERT_THREAD 3
#define INSERT_LEAF 4
#define INSERT_EMPTY 5

#define MAX_KEY_PREFIX_LEN 20

class dfox_iterator_status {
public:
    byte *t;
    byte tp[MAX_KEY_PREFIX_LEN];
    byte tc_a[MAX_KEY_PREFIX_LEN];
    byte child_a[MAX_KEY_PREFIX_LEN];
    byte leaf_a[MAX_KEY_PREFIX_LEN];
    byte offset_a[MAX_KEY_PREFIX_LEN];
};

class dfox_node_handler {
private:
    static byte left_mask[8];
    static byte left_incl_mask[8];
    static byte ryte_mask[8];
    static byte ryte_incl_mask[8];
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
    inline byte insAt(byte *ptr, byte b1, byte b2, byte b3);
    inline byte ins4BytesAt(byte *ptr, byte b1, byte b2, byte b3, byte b4);
    inline byte insChildAndLeafAt(byte *ptr, byte b1, byte b2);
    inline void setAt(byte pos, byte b);
    inline void append(byte b);
    inline byte getAt(byte pos);
    inline void delAt(byte *ptr);
    inline void delAt(byte *ptr, int16_t count);
    inline void insBit(uint32_t *ui32, int pos, int16_t kv_pos);
    inline void insBit(uint64_t *ui64, int pos, int16_t kv_pos);
    int16_t findPos(dfox_iterator_status& s, int brk_idx);
    int16_t nextKey(dfox_iterator_status& s);
    void deleteTrieLastHalf(int16_t brk_key_len, dfox_iterator_status& s);
    void deleteTrieFirstHalf(int16_t brk_key_len, dfox_iterator_status& s);
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
    int16_t key_at_pos;
    const char *value;
    int16_t value_len;
#if defined(DX_INT64MAP)
    uint64_t *bitmap;
#else
    uint32_t *bitmap1;
    uint32_t *bitmap2;
#endif
    byte *skip_list[16];
    dfox_node_handler(byte *m);
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
    inline byte *getKey(int16_t pos, int16_t *plen);
    byte *getData(int16_t pos, int16_t *plen);
    byte *split(int16_t *pbrk_idx, byte *first_key, int16_t *first_len_ptr);
    inline int16_t getPtr(int16_t pos);
    inline void setPtr(int16_t pos, int16_t ptr);
    void insPtr(int16_t pos, int16_t kvIdx);
    int16_t locate(int16_t level);
    void insertCurrent();
};

class dfox {
private:
    long total_size;
    int numLevels;
    int maxKeyCount;
    int blockCount;
    void recursiveSearch(dfox_node_handler *node, int16_t lastSearchPos[],
            byte *node_paths[], int16_t *pIdx);
    void recursiveSearchForGet(dfox_node_handler *node, int16_t *pIdx);
    void recursiveUpdate(dfox_node_handler *node, int16_t pos,
            int16_t lastSearchPos[], byte *node_paths[], int16_t level);
public:
    byte *root_data;
    int maxThread;
    dfox();
    ~dfox();
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
    }
    long size() {
        return total_size;
    }
};

#endif
