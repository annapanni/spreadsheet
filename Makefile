CXX = g++
CXXFLAGS = -Werror -Wall -Wextra -Wpedantic -Wconversion -fsanitize=address

SRCS = token.cpp expression.cpp sheet.cpp parser.cpp
OBJS = $(SRCS:.cpp=.o)

SRCS1 = main.cpp
OBJS1 = $(OBJS) $(SRCS1:.cpp=.o)
PROG1 = test

SRCS2 = ui.cpp
OBJS2 = $(OBJS) $(SRCS2:.cpp=.o)
PROG2 = console

TARGETS = $(PROG1) $(PROG2)

$(PROG1): $(OBJS1)
	$(CXX) $(CXXFLAGS) $^ -o $@

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
