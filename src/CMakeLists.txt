
CMAKE_MINIMUM_REQUIRED (VERSION 2.8)
PROJECT (SHINY-AVENGER)

ADD_SUBDIRECTORY (commands)
ADD_SUBDIRECTORY (algorithms)

# create main shiny-avenger executable
ADD_EXECUTABLE (shiny-avenger main.c teams.c results.c parse_ncaa.c)
TARGET_LINK_LIBRARIES (shiny-avenger m commands algorithms)
