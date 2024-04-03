CXX = g++
CXXFLAGS = -Werror -Wall -Wextra -Wpedantic -Wconversion -fsanitize=address

SRCS = token.cpp expression.cpp sheet.cpp parser.cpp main.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = main

all: $(TARGET)

$(TARGET) : $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm $(OBJS)

run:
	./$(TARGET)

again:
	make clean
	make all
