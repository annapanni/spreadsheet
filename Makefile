CXX = g++
CXXFLAGS = -Werror -Wall -Wextra -Wpedantic -Wconversion -fsanitize=address
GTTESTFLAGS = -lgtest -lgtest_main

SRCS = srcs/token.cpp srcs/expression.cpp srcs/sheet.cpp srcs/parser.cpp srcs/console.cpp
OBJS = $(SRCS:.cpp=.o)

SRCS1 = srcs/test.cpp
OBJS1 = $(OBJS) $(SRCS1:.cpp=.o)

SRCS2 = srcs/main.cpp
OBJS2 = $(OBJS) $(SRCS2:.cpp=.o)


test: $(OBJS1)
	$(CXX) $^ $(CXXFLAGS) $(GTTESTFLAGS) -o $@

console: $(OBJS2)
	$(CXX) $(CXXFLAGS) $^ -o $@

#$(TARGET) : $(OBJS)
#	$(CXX) $(CXXFLAGS) $^ -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS1) $(OBJS2) test console

again:
	make clean
	make console
