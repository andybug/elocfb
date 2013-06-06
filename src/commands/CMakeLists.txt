
CMAKE_MINIMUM_REQUIRED (VERSION 2.8)
PROJECT (ELOCFB)

# build commands library
ADD_LIBRARY (commands init.c parse.c process.c algo.c)
