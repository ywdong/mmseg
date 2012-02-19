g++ -shared -fPIC wrap_mmseg.cc -o mmseg.so -lboost_python \
-I/usr/include/python2.6 -I/usr/local/include/mmseg -lmmseg -L/usr/local/lib

cp mmseg.so /usr/local/lib/python2.6/dist-packages/