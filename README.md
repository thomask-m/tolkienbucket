## tolkien bucket

An exercise in C++ concurrent programming (and learning C++ in general).

The bucket offers a non-boolean `take` API that differs from the traditional boolean `take` of a "token" bucket - threads are blocked until the bucket is refilled.

Playing around with a fill rate based on seconds rather than a smaller unit of time so I can observe its behavior with my human eyes.

### build and run ./validate
```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER=/usr/bin/g++ ..
cmake --build .
./validate
```

