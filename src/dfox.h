#ifndef dfox_H
#define dfox_H
#include <cstdio>
#include <cstring>
#include <iostream>
#include "util.h"
#include "bp_tree.h"

using namespace std;

typedef unsigned char byte;
#define DFOX_NODE_SIZE 512
#define IDX_BLK_SIZE 64
#define IDX_HDR_SIZE 8
#define TRIE_PTR_AREA_SIZE 56

#define INSERT_MIDDLE1 1
#define INSERT_MIDDLE2 2
#define INSERT_THREAD 3
#define INSERT_LEAF1 4
#define INSERT_LEAF2 5

#define IS_LEAF_BYTE buf[3]
#define DATA_PTR_HIGH_BITS buf
#define TRIE_LEN buf[4]
#define FILLED_SIZE buf[5]
#define LAST_DATA_PTR buf + 6

class dfox_var: public bplus_tree_var {
public:
    byte tc;
    byte mask;
    byte msb5;
    byte csPos;
    byte leaves;
    byte triePos;
    byte origPos;
    byte lastLevel;
    byte need_count;
    byte insertState;
    char *key_at;
    int key_at_len;
    dfox_var() {
        init();
    }
    ~dfox_var() {
    }
    void init();
};

class dfox_node: public bplus_tree_node {
private:
    byte *trie;
    static byte left_mask[8];
    static byte ryte_mask[8];
    static byte ryte_incl_mask[8];
    static byte pos_mask[32];
    inline void insAt(byte pos, byte b);
    inline void insAt(byte pos, byte b1, byte b2);
    inline void insAt(byte pos, byte b1, byte b2, byte b3);
    inline void setAt(byte pos, byte b);
    inline void append(byte b);
    inline byte getAt(byte pos);
    inline void delAt(byte pos);
    inline void delAt(byte pos, int count);
    byte recurseSkip(dfox_var *v, byte skip_count, byte skip_size);
public:
    dfox_node();
    bool isFull(int kv_len, dfox_var *v);
    bool isFull(int kv_len, bplus_tree_var *v);
    inline bool isLeaf();
    inline void setLeaf(char isLeaf);
    int filledSize();
    inline void setFilledSize(int filledSize);
    inline int getKVLastPos();
    inline void setKVLastPos(int val);
    void addData(int idx, const char *value, int value_len, dfox_var *v);
    void addData(int idx, const char *value, int value_len, bplus_tree_var *v);
    dfox_node *getChild(int pos);
    byte *getKey(int pos, int *plen);
    byte *getData(int pos, int *plen);
    dfox_node *split(int *pbrk_idx);
    int getPtr(int pos);
    void insPtr(int pos, int kvIdx);
    int locate(dfox_var *v);
    int locate(bplus_tree_var *v);
    bool recurseTrie(int level, dfox_var *v);
    byte recurseEntireTrie(int level, dfox_var *v, long idx_list[],
            int *pidx_len);
    void insertCurrent(dfox_var *v);
};

class dfox: public bplus_tree {
private:
    dfox_node *newNode();
    dfox_var *newVar();
public:
    dfox();

};

#endif
