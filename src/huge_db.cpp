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
using namespace std::tr1;
using namespace std;

void insert(unordered_map<string, string>& m) {
    char k[100];
    char v[100];
    srand(time(NULL));
    for (long l = 0; l < 164000; l++) {
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
    cout << "HashMap size:" << m.size() << endl;

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

    return 0;

}
