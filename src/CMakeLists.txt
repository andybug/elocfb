
CMAKE_MINIMUM_REQUIRED (VERSION 2.8)
PROJECT (SHINY-AVENGER)

ADD_SUBDIRECTORY (commands)

# create main shiny-avenger executable
ADD_EXECUTABLE (shiny-avenger main.c)
TARGET_LINK_LIBRARIES (shiny-avenger commands)
