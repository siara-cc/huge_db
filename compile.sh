g++ -I./hdr -I./src -O3 -Wall -c -fmessage-length=0 -o Release/src/huge_db.o ./src/huge_db.cpp
g++ -I./hdr -I./src -O3 -Wall -c -fmessage-length=0 -o Release/src/art.o ./src/art.cpp
g++ -I./hdr -I./src -O3 -Wall -c -fmessage-length=0 -o Release/src/rb_tree.o ./src/rb_tree.cpp
g++ -I./hdr -I./src -O3 -Wall -c -fmessage-length=0 -o Release/src/dfox.o ./src/dfox.cpp
g++ -I./hdr -I./src -O3 -Wall -c -fmessage-length=0 -o Release/src/dfos.o ./src/dfos.cpp
g++ -I./hdr -I./src -O3 -Wall -c -fmessage-length=0 -o Release/src/dfqx.o ./src/dfqx.cpp
g++ -I./hdr -I./src -O3 -Wall -c -fmessage-length=0 -o Release/src/bfos.o ./src/bfos.cpp
g++ -I./hdr -I./src -O3 -Wall -c -fmessage-length=0 -o Release/src/bft.o ./src/bft.cpp
g++ -I./hdr -I./src -O3 -Wall -c -fmessage-length=0 -o Release/src/dft.o ./src/dft.cpp
g++ -I./hdr -I./src -O3 -Wall -c -fmessage-length=0 -o Release/src/GenTree.o ./src/GenTree.cpp
g++ -I./hdr -I./src -O3 -Wall -c -fmessage-length=0 -o Release/src/linex.o ./src/linex.cpp
g++ -I./hdr -I./src -O3 -Wall -c -fmessage-length=0 -o Release/src/basix.o ./src/basix.cpp
g++ -I./hdr -I./src -O3 -Wall -c -fmessage-length=0 -o Release/src/univix_util.o ./src/univix_util.cpp
cd Release
g++ -o ../huge_db src/rb_tree.o src/basix.o src/huge_db.o src/dfox.o src/dfos.o src/dfqx.o src/linex.o src/bfos.o src/dft.o src/bft.o src/art.o src/GenTree.o src/univix_util.o
