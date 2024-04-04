CXX = g++
CXXFLAGS = -Werror -Wall -Wextra -Wpedantic -Wconversion -fsanitize=address
GTTESTFLAGS = -lgtest -lgtest_main

SRCS = token.cpp expression.cpp sheet.cpp parser.cpp
OBJS = $(SRCS:.cpp=.o)

SRCS1 = test.cpp
OBJS1 = $(OBJS) $(SRCS1:.cpp=.o)

SRCS2 = ui.cpp
OBJS2 = $(OBJS) $(SRCS2:.cpp=.o)
PROG2 = console

TARGETS = $(PROG1) $(PROG2)

test: $(OBJS1)
	$(CXX) $^ $(CXXFLAGS) $(GTTESTFLAGS) -o $@

$(PROG2): $(OBJS2)
	$(CXX) $(CXXFLAGS) $^ -o $@

#$(TARGET) : $(OBJS)
#	$(CXX) $(CXXFLAGS) $^ -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS1) $(OBJS2)

again:
	make clean
	make console
