CXX = g++
CXXFLAGS = -std=c++11 -Wall -g -Wextra -pthread -Iinclude
LDFLAGS = -lpthread -lmysqlclient -lcurl -llog4cpp 

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = .
LOG_DIR = logs

SRCS = $(wildcard $(SRC_DIR)/*.cc)
OBJS = $(patsubst $(SRC_DIR)/%.cc,$(OBJ_DIR)/%.o,$(SRCS))
TARGET = $(BIN_DIR)/SmartHomeMonitorServer

CONF_DIR = conf
CONF_FILE = $(CONF_DIR)/server.conf

.PHONY: all clean

all: $(TARGET) $(CONF_FILE)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(TARGET) $(wildcard $(OBJ_DIR)/*.o)

