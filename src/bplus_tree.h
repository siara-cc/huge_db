#ifndef BP_TREE_H
#define BP_TREE_H
#ifdef ARDUINO
#include <HardwareSerial.h>
#include <string.h>
#else
#include <cstdio>
#include <cstring>
#include <iostream>
#endif
#include <stdint.h>
#include "univix_util.h"

using namespace std;

#define BPT_IS_LEAF_BYTE buf[0]
#define BPT_FILLED_SIZE buf + 1
#define BPT_LAST_DATA_PTR buf + 3
#define BPT_MAX_KEY_LEN buf[5]
#define BPT_TRIE_LEN buf[6]

#ifdef ARDUINO
#define BPT_INT64MAP 0
#else
#define BPT_INT64MAP 1
#endif
#define BPT_9_BIT_PTR 0

class bplus_tree_node_handler {
public:
    byte *buf;
    const char *key;
    byte key_len;
    byte *key_at;
    byte key_at_len;
    const char *value;
    int16_t value_len;
#if BPT_9_BIT_PTR == 1
#if BPT_INT64MAP == 1
    uint64_t *bitmap;
#else
    uint32_t *bitmap1;
    uint32_t *bitmap2;
#endif
#endif
    virtual ~bplus_tree_node_handler() {
    }
    virtual void setBuf(byte *m) = 0;
    virtual void initBuf() = 0;
    virtual void initVars() = 0;
    inline bool isLeaf() {
        return BPT_IS_LEAF_BYTE;
    }

    inline void setLeaf(char isLeaf) {
        BPT_IS_LEAF_BYTE = isLeaf;
    }

    inline void setFilledSize(int16_t filledSize) {
        util::setInt(BPT_FILLED_SIZE, filledSize);
    }

    inline int16_t filledSize() {
        return util::getInt(BPT_FILLED_SIZE);
    }

    inline void setKVLastPos(uint16_t val) {
        util::setInt(BPT_LAST_DATA_PTR, val);
    }

    inline uint16_t getKVLastPos() {
        return util::getInt(BPT_LAST_DATA_PTR);
    }

    byte *getKey(int16_t pos, byte *plen) {
        byte *kvIdx = buf + getPtr(pos);
        *plen = *kvIdx;
        return kvIdx + 1;
    }

    byte *getChildPtr(byte *ptr) {
        ptr += (*ptr + 1);
        return (byte *) util::bytesToPtr(ptr);
    }

    char *getValueAt(int16_t *vlen) {
        key_at += key_at_len;
        *vlen = *key_at;
        return (char *) key_at + 1;
    }

    uint16_t getPtr(int16_t pos) {
#if BPT_9_BIT_PTR == 1
        uint16_t ptr = *(getPtrPos() + pos);
#if BPT_INT64MAP == 1
        if (*bitmap & MASK64(pos))
        ptr |= 256;
#else
        if (pos & 0xFFE0) {
            if (*bitmap2 & MASK32(pos - 32))
            ptr |= 256;
        } else {
            if (*bitmap1 & MASK32(pos))
            ptr |= 256;
        }
#endif
        return ptr;
#else
        return util::getInt(getPtrPos() + (pos << 1));
#endif
    }

    void insPtr(int16_t pos, uint16_t kv_pos) {
        int16_t filledSz = filledSize();
#if BPT_9_BIT_PTR == 1
        byte *kvIdx = getPtrPos() + pos;
        memmove(kvIdx + 1, kvIdx, filledSz - pos);
        *kvIdx = kv_pos;
#if BPT_INT64MAP == 1
        insBit(bitmap, pos, kv_pos);
#else
        if (pos & 0xFFE0) {
            insBit(bitmap2, pos - 32, kv_pos);
        } else {
            byte last_bit = (*bitmap1 & 0x01);
            insBit(bitmap1, pos, kv_pos);
            *bitmap2 >>= 1;
            if (last_bit)
            *bitmap2 |= MASK32(0);
        }
#endif
#else
        byte *kvIdx = getPtrPos() + (pos << 1);
        memmove(kvIdx + 2, kvIdx, (filledSz - pos) * 2);
        util::setInt(kvIdx, kv_pos);
#endif
        setFilledSize(filledSz + 1);

    }

    void setPtr(int16_t pos, uint16_t ptr) {
#if BPT_9_BIT_PTR == 1
        *(getPtrPos() + pos) = ptr;
#if BPT_INT64MAP == 1
        if (ptr >= 256)
        *bitmap |= MASK64(pos);
        else
        *bitmap &= ~MASK64(pos);
#else
        if (pos & 0xFFE0) {
            pos -= 32;
            if (ptr >= 256)
            *bitmap2 |= MASK32(pos);
            else
            *bitmap2 &= ~MASK32(pos);
        } else {
            if (ptr >= 256)
            *bitmap1 |= MASK32(pos);
            else
            *bitmap1 &= ~MASK32(pos);
        }
#endif
#else
        byte *kvIdx = getPtrPos() + (pos << 1);
        return util::setInt(kvIdx, ptr);
#endif
    }

    void insBit(uint32_t *ui32, int pos, uint16_t kv_pos) {
        uint32_t ryte_part = (*ui32) & RYTE_MASK32(pos);
        ryte_part >>= 1;
        if (kv_pos >= 256)
            ryte_part |= MASK32(pos);
        (*ui32) = (ryte_part | ((*ui32) & LEFT_MASK32(pos)));

    }

#if BPT_INT64MAP == 1
    void insBit(uint64_t *ui64, int pos, uint16_t kv_pos) {
        uint64_t ryte_part = (*ui64) & RYTE_MASK64(pos);
        ryte_part >>= 1;
        if (kv_pos >= 256)
            ryte_part |= MASK64(pos);
        (*ui64) = (ryte_part | ((*ui64) & LEFT_MASK64(pos)));

    }
#endif

    virtual bool isFull(int16_t kv_len) = 0;
    virtual void addData() = 0;
    virtual int16_t traverseToLeaf(byte *node_paths[] = null) = 0;
    virtual int16_t locate() = 0;
    virtual byte *split(byte *first_key, int16_t *first_len_ptr) = 0;
    virtual byte *getPtrPos() = 0;
};

class trie_node_handler: public bplus_tree_node_handler {
protected:
    const static byte need_counts[10];
    virtual void decodeNeedCount() = 0;

    inline void delAt(byte *ptr) {
        BPT_TRIE_LEN--;
        memmove(ptr, ptr + 1, trie + BPT_TRIE_LEN - ptr);
    }

    inline void delAt(byte *ptr, int16_t count) {
        BPT_TRIE_LEN -= count;
        memmove(ptr, ptr + count, trie + BPT_TRIE_LEN - ptr);
    }

    inline byte insAt(byte *ptr, byte b) {
        memmove(ptr + 1, ptr, trie + BPT_TRIE_LEN - ptr);
        *ptr = b;
        BPT_TRIE_LEN++;
        return 1;
    }

    inline byte insAt(byte *ptr, byte b1, byte b2) {
        memmove(ptr + 2, ptr, trie + BPT_TRIE_LEN - ptr);
        *ptr++ = b1;
        *ptr = b2;
        BPT_TRIE_LEN += 2;
        return 2;
    }

    inline byte insAt(byte *ptr, byte b1, byte b2, byte b3) {
        memmove(ptr + 3, ptr, trie + BPT_TRIE_LEN - ptr);
        *ptr++ = b1;
        *ptr++ = b2;
        *ptr = b3;
        BPT_TRIE_LEN += 3;
        return 3;
    }

    inline byte insAt(byte *ptr, byte b1, byte b2, byte b3, byte b4) {
        memmove(ptr + 4, ptr, trie + BPT_TRIE_LEN - ptr);
        *ptr++ = b1;
        *ptr++ = b2;
        *ptr++ = b3;
        *ptr = b4;
        BPT_TRIE_LEN += 4;
        return 4;
    }

    inline void insAt(byte *ptr, byte b, const char *s, byte len) {
        memmove(ptr + 1 + len, ptr, trie + BPT_TRIE_LEN - ptr);
        *ptr++ = b;
        memcpy(ptr, s, len);
        BPT_TRIE_LEN += len;
        BPT_TRIE_LEN++;
    }

    inline void insAt(byte *ptr, const char *s, byte len) {
        memmove(ptr + len, ptr, trie + BPT_TRIE_LEN - ptr);
        memcpy(ptr, s, len);
        BPT_TRIE_LEN += len;
    }

    void insBytes(byte *ptr, int16_t len) {
        memmove(ptr + len, ptr, trie + BPT_TRIE_LEN - ptr);
        BPT_TRIE_LEN += len;
    }

    inline void setAt(byte pos, byte b) {
        trie[pos] = b;
    }

    inline void append(byte b) {
        trie[BPT_TRIE_LEN++] = b;
    }

    inline void append(byte b1, byte b2) {
        trie[BPT_TRIE_LEN++] = b1;
        trie[BPT_TRIE_LEN++] = b2;
    }

    inline void append(byte b1, byte b2, byte b3) {
        trie[BPT_TRIE_LEN++] = b1;
        trie[BPT_TRIE_LEN++] = b2;
        trie[BPT_TRIE_LEN++] = b3;
    }

    inline void appendPtr(int16_t p) {
        util::setInt(trie + BPT_TRIE_LEN, p);
        BPT_TRIE_LEN += 2;
    }

    void append(const char *s, int16_t need_count) {
        memcpy(trie + BPT_TRIE_LEN, s, need_count);
        BPT_TRIE_LEN += need_count;
    }

public:
    static const byte x00 = 0;
    static const byte x01 = 1;
    static const byte x02 = 2;
    static const byte x03 = 3;
    static const byte x04 = 4;
    static const byte x05 = 5;
    static const byte x06 = 6;
    static const byte x07 = 7;
    static const byte x08 = 8;
    static const byte x0F = 0x0F;
    static const byte x10 = 0x10;
    static const byte x11 = 0x11;
    static const byte x3F = 0x3F;
    static const byte x40 = 0x40;
    static const byte x41 = 0x41;
    static const byte x55 = 0x55;
    static const byte x7F = 0x7F;
    static const byte x80 = 0x80;
    static const byte x81 = 0x81;
    static const byte xAA = 0xAA;
    static const byte xBF = 0xBF;
    static const byte xC0 = 0xC0;
    static const byte xF8 = 0xF8;
    static const byte xFB = 0xFB;
    static const byte xFC = 0xFC;
    static const byte xFD = 0xFD;
    static const byte xFE = 0xFE;
    static const byte xFF = 0xFF;
    static const int16_t x100 = 0x100;
    byte *trie;
    byte *triePos;
    byte *origPos;
    byte need_count;
    byte insertState;
    byte keyPos;
    virtual ~trie_node_handler() {}
};

class bplus_tree {
private:
    void recursiveUpdate(bplus_tree_node_handler *node, int16_t pos,
            byte *node_paths[], int16_t level);
protected:
    long total_size;
    int numLevels;
    int maxKeyCountNode;
    int maxKeyCountLeaf;
    int maxTrieLenNode;
    int maxTrieLenLeaf;
    int blockCountNode;
    int blockCountLeaf;

public:
    byte *root_data;
    int maxThread;
    bplus_tree() {
        numLevels = blockCountNode = blockCountLeaf = maxKeyCountNode =
                maxKeyCountLeaf = total_size = maxThread = 0;
        root_data = NULL;
    }
    virtual ~bplus_tree() {
    }
    virtual void put(const char *key, int16_t key_len, const char *value,
            int16_t value_len) = 0;
    virtual char *get(const char *key, int16_t key_len, int16_t *pValueLen) = 0;
    void printMaxKeyCount(long num_entries) {
        util::print("Block Count:");
        util::print((long) blockCountNode);
        util::print(", ");
        util::print((long) blockCountLeaf);
        util::endl();
        util::print("Avg Block Count:");
        util::print((long) (num_entries / blockCountLeaf));
        util::endl();
        util::print("Avg Max Count:");
        util::print((long) (maxKeyCountNode / (blockCountNode ? blockCountNode : 1)));
        util::print(", ");
        util::print((long) (maxKeyCountLeaf / blockCountLeaf));
        util::endl();
        util::print("Avg Trie Len:");
        util::print((long) (maxTrieLenNode / (blockCountNode ? blockCountNode : 1)));
        util::print(", ");
        util::print((long) (maxTrieLenLeaf / blockCountLeaf));
        util::endl();
    }
    void printNumLevels() {
        util::print("Level Count:");
        util::print((long) numLevels);
    }
    long size() {
        return total_size;
    }

};

#endif
