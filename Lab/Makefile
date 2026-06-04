CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -O3 -fopenmp
INCLUDES = -I./include
LDFLAGS = 

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SOURCES))
TARGET = $(BIN_DIR)/mpemba_sim

.PHONY: all clean run plots gifs dirs

all: dirs $(TARGET)

dirs:
	mkdir -p $(OBJ_DIR) $(BIN_DIR) output

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

run: all
	./$(TARGET)

plots:
	python3 scripts/plot_curvas.py
	python3 scripts/plot_mpemba.py

gifs:
	python3 scripts/crear_gif.py

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR) output/*
