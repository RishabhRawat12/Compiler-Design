CXX = g++
# Compile position-independent code to avoid PIE linking errors on some Linux distros
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra -fPIC
LDFLAGS = -no-pie
TARGET = analyzer

SRCS = main.cpp \
       lexer/lexical_analyzer.cpp \
       parser/syntax_analyzer.cpp \
       semantic/semantic_analyzer.cpp \
       intermediate/intermediate_code.cpp

OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
