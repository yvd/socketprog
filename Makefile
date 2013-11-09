# Programs
CC     	= g++
RM 	= rm
ECHO	= echo
######################################
# Project Name (generate executable with this name)
TARGET = filemesh

# Project Paths
PROJECT_DIR=./
SRC_DIR=$(PROJECT_DIR)
#Libraries

SRC := $(wildcard $(SRC_DIR)/*.cpp)

all: $(PROJECT_DIR)/$(TARGET)

$(PROJECT_DIR)/$(TARGET): $(SRC)
	@$(CC) -c client.cpp
	@$(CC) -o $(TARGET) node.cpp	
	
clean:
	@$(ECHO) -n "Cleaning..."
	@$(RM) -rf $(TARGET) *~ a.out *.o
	@$(ECHO) "Done"