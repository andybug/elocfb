
CMAKE_MINIMUM_REQUIRED (VERSION 2.8)
PROJECT (SHINY-AVENGER)

# create main shiny-avenger executable
ADD_LIBRARY (commands init.c parse.c process.c)
