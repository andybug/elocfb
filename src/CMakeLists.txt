
CMAKE_MINIMUM_REQUIRED (VERSION 2.8)
PROJECT (ELOCFB)

ADD_SUBDIRECTORY (commands)
ADD_SUBDIRECTORY (algorithms)

# create main elocfb executable
ADD_EXECUTABLE (elocfb main.c teams.c results.c parse_ncaa.c database.c output.c)
TARGET_LINK_LIBRARIES (elocfb m pq commands algorithms)
