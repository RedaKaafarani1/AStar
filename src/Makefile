CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++20
LDFLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

SRC = Main.cpp AStar.cpp Entity.cpp
OBJ = $(SRC:.cpp=.o)

TARGET = astar

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f $(OBJ) $(TARGET)

run: $(TARGET)
	./$(TARGET)
