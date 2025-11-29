CXX=g++
CXXFLAGS=-O3 -Wall -std=c++17

ROOT_DIR=..
STRATEGIES_DIR=./strategies
TYPES_DIR=./types

GOOGLETEST_DIR=/opt/homebrew/opt/googletest

INCLUDES= -I. \
		  -I$(STRATEGIES_DIR) \
		  -I$(TYPES_DIR) \
		  -I$(GOOGLETEST_DIR)/include


LDFLAGS= -L$(GOOGLETEST_DIR)/lib 


LIBS= -pthread \
	-lgtest \
	-lgtest_main

TARGET=DBacktest

OTHER_SRCS=$(filter-out main.cpp, $(wildcard *.cpp))
STRATEGY_SRCS=$(wildcard $(STRATEGIES_DIR)/*.cpp)

SRCS = main.cpp $(OTHER_SRCS) $(STRATEGY_SRCS) $(CLIENT_SRCS)
TEST_SRCS = $(OTHER_SRCS) $(STRATEGY_SRCS) $(CLIENT_SRCS) tests/*.cpp
INPUTS= crypto coinbase btc-usd 15m output.txt

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SRCS) $(LDFLAGS) $(LIBS) -o $(TARGET)

run: $(TARGET)
	./$(TARGET) $(INPUTS)

test: $(TARGET)_TEST

run_test: $(TARGET)_TEST
	./$(TARGET)_TEST $(INPUTS)

$(TARGET)_TEST: $(SRCS) tests/*.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(TEST_SRCS) $(LDFLAGS) $(LIBS) -o $(TARGET)_TEST

clean:
	rm -f $(TARGET) $(TARGET)_TEST
