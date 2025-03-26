
CC = cc
CXX = c++
#CFLAGS := -std=c11 -pthread -fPIC -O2 -Wall
CFLAGS := -std=c11 -pthread -fPIC -g -Wall
#CXXFLAGS := -std=c++17 -pthread -fPIC -O2 -Wall
CXXFLAGS := -std=c++17 -pthread -fPIC -g -Wall
CINC = pkg-config gtkmm-3.0 openssl --cflags
CLIB = pkg-config gtkmm-3.0 openssl --libs
TARGET = kbox
SRCS = main.cpp kbox.cpp kwin.cpp
OBJS = $(SRCS:.cpp=.o)
SRCS_0 = ksubc.c crypt-des.c
OBJS_0 = $(SRCS_0:.c=.o)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $< `$(CINC)`

.c.o:
	$(CC) $(CFLAGS) -c $< `$(CINC)`

$(TARGET): $(OBJS) $(OBJS_0)
	$(CXX) $(CXXFLAGS) -o $@ $^ `$(CLIB)`

clean:
	-rm $(TARGET) $(OBJS) $(OBJS_0)

