# Project Name
TARGET = that_simple



# Sources
C_INCLUDES = -Icore
CPP_SOURCES = that_simple.cpp $(wildcard core/*.cpp) $(wildcard control/*.cpp)

# Library Locations
LIBDAISY_DIR = ../DaisyExamples/libDaisy/
DAISYSP_DIR = ../DaisyExamples/DaisySP/

# Core location, and generic Makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile
