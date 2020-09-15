NAME = gl-shaders
CXX = g++
CXXFLAGS = -O3
LDFLAGS = -lm -lrt -lGLEW -lGL -lGLU -lX11
SRCS = main.cpp events.cpp graphics.cpp
OBJS = $(SRCS:%.cpp=%.o)
HDRS = config.hpp main.hpp

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(LDFLAGS) $(OBJS) -o $(NAME)

.cpp.o:
	$(CXX) $(CFLAGS) -c $< -o $@

$(OBJS): $(HDRS)

clean:
	rm -f $(OBJS)
	rm -f $(NAME)
