#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <iostream>
#include <cmath>
#include <string>
#include <vector>
#include <map>
#include <tr1/unordered_map>
#include "art.h"
#include "linex.h"
using namespace std::tr1;
using namespace std;

void insert(unordered_map<string, string>& m) {
    char k[100];
    char v[100];
    srand(time(NULL));
    for (long l = 0; l < 655360; l++) {
        long r = rand() * rand();
        for (int b = 0; b < 4; b++) {
            char c = (r >> (24 - b * 8));
            k[b * 2] = 48 + (c >> 4);
            k[b * 2 + 1] = 48 + (c & 0x0F);
        }
        r = rand() * rand();
        for (int b = 4; b < 8; b++) {
            char c = (r >> (24 - (b - 4) * 8));
            k[b * 2] = 48 + (c >> 4);
            k[b * 2 + 1] = 48 + (c & 0x0F);
        }
        k[16] = 0;
        itoa(rand(), v, 10);
        itoa(rand(), v + strlen(v), 10);
        itoa(rand(), v + strlen(v), 10);
        if (l == 0)
            cout << "key:" << k << endl;
        m.insert(pair<string, string>(k, v));
    }
}
float timedifference_msec(struct timeval t0, struct timeval t1) {
    return (t1.tv_sec - t0.tv_sec) * 1000.0f
            + (t1.tv_usec - t0.tv_usec) / 1000.0f;
}

int main1() {
    linex *lx = new linex();
    lx->put("Hello", 5, "World", 5);
    lx->put("Nice", 4, "Place", 5);
    lx->put("Arun", 4, "Hello", 5);
    lx->put("arun", 4, "0", 1);
    lx->put("ri0hello", 7, "2", 1);
    lx->put("ricin", 5, "3", 1);
    lx->put("rickshaw", 8, "4", 1);
    lx->put("ride", 4, "5", 1);
    lx->put("rider", 5, "6", 1);
    lx->put("rice", 4, "7", 1);
    lx->put("ric", 3, "8", 1);
    lx->put("aruna", 5, "9", 1);
    lx->put("hello", 5, "10", 2);
    lx->put("world", 5, "11", 2);
    lx->put("how", 3, "12", 2);
    lx->put("are", 3, "13", 2);
    lx->put("1", 1, "15", 2);
    lx->put("you", 3, "14", 2);
    lx->put("boy", 3, "15", 2);
    int len;
    char *value = lx->get("Nice", 4, &len);
    char s[100];
    strncpy(s, value, 5);
    s[5] = 0;
    std::cout << "Value:" << s << endl;
    return 0;
}

int main2() {
    linex *lx = new linex();
    lx->put("0?391>63381307(0", 16, "1341012271188", 13);
    lx->put("08/135/:03*6200<", 16, "2458735612129", 13);
    lx->put("0?5;123300,=(1*4", 16, "236482090128166", 15);
    lx->put("15*<.6,618/21816", 16, "11814303447566", 14);
    lx->put("0717.<040500*066", 16, "3673237115013", 13);
    lx->put("0=,0180;1;(=/52;", 16, "145312276917216", 15);
    lx->put("04/=,;.60<+413.8", 16, "1993614936778", 13);
    lx->put("0417+:581;307:6:", 16, "306762304331492", 15);
    lx->put("00)3(<.802/;*<3=", 16, "250811475410158", 15);
    lx->put("067;*6.007+64956", 16, "156701999810315", 15);
    char *value = null;
    int len;
    value = lx->get("04/=,;.60<+413.8", 16, &len);
    char v[100];
    memset(v, 0, sizeof(v));
    strncpy(v, value, len);
    cout << v << endl;
    return 0;
}

int main() {

    /*
     db d("/tmp", "test");
     rs r(&d, "table1");
     r.insert("record1", 7);
     */
    struct timeval stop, start;
    unordered_map<string, string> m;
    gettimeofday(&start, NULL);
    insert(m);
    gettimeofday(&stop, NULL);
    cout << "HashMap insert time:" << timedifference_msec(start, stop) << endl;
    cout << "HashMap size:" << m.size() << endl;
    map<string, string> m1;
    unordered_map<string, string>::iterator it = m.begin();
    gettimeofday(&start, NULL);
    for (; it != m.end(); ++it) {
        m1.insert(pair<string, string>(it->first, it->second));
    }
    gettimeofday(&stop, NULL);
    cout << "RB Tree insert time:" << timedifference_msec(start, stop) << endl;
    it = m.begin();
    gettimeofday(&start, NULL);
    for (; it != m.end(); ++it) {
        m1[it->first];
    }
    gettimeofday(&stop, NULL);
    cout << "RB Tree get time:" << timedifference_msec(start, stop) << endl;
    cout << "RB Tree size:" << m1.size() << endl;

    art_tree at;
    art_tree_init(&at);
    gettimeofday(&start, NULL);
    it = m.begin();
    for (; it != m.end(); ++it) {
        art_insert(&at, (unsigned char*) it->first.c_str(), it->first.length(),
                (void *) it->second.c_str());
    }
    gettimeofday(&stop, NULL);
    cout << "ART Insert Time:" << timedifference_msec(start, stop) << endl;
    gettimeofday(&start, NULL);
    it = m.begin();
    for (; it != m.end(); ++it) {
        art_search(&at, (unsigned char*) it->first.c_str(), it->first.length());
    }
    gettimeofday(&stop, NULL);
    cout << "ART Get Time:" << timedifference_msec(start, stop) << endl;
    cout << "ART Size:" << art_size(&at) << endl;

    linex *lx = new linex();
    it = m.begin();
    gettimeofday(&start, NULL);
    for (; it != m.end(); ++it) {
        lx->put(it->first.c_str(), it->first.length(), it->second.c_str(),
                it->second.length());
    }
    gettimeofday(&stop, NULL);
    cout << "B+Tree insert time:" << timedifference_msec(start, stop) << endl;

    it = m.begin();
    int ctr = 0;
    int cmp = 0;
    gettimeofday(&start, NULL);
    for (; it != m.end(); ++it) {
        int len;
        char *value = lx->get(it->first.c_str(), it->first.length(), &len);
        char v[100];
        if (value == null) {
            ctr++;
        } else {
            int d = linex_block::compare(it->second.c_str(),
                    it->second.length(), value, len);
            if (d != 0) {
                cmp++;
                strncpy(v, value, len);
                v[it->first.length()] = 0;
                cout << cmp << ":" << it->first.c_str() << "=========="
                        << it->second.c_str() << "----------->" << v << endl;
            }
        }
    }
    gettimeofday(&stop, NULL);
    cout << "Null:" << ctr << endl;
    cout << "Cmp:" << cmp << endl;
    cout << "B+Tree get time:" << timedifference_msec(start, stop) << endl;

    if (cmp > 0) {
        it = m.begin();
        for (; it != m.end(); ++it) {
            cout << "\"" << it->first.c_str() << "\", \"" << it->second.c_str()
                    << "\"," << endl;
        }
    }

    return 0;

}
