BDIR = Build
SDIR = Source

EXECUTABLE = $(BDIR)/Streaming

SOURCES = $(wildcard $(SDIR)/*.cpp)

DEPENDENCIES = $(wildcard $(SDIR)/*.h) \

_OBJECTS = $(SOURCES:.cpp=.o)
OBJECTS = $(patsubst $(SDIR)/%,$(BDIR)/%,$(_OBJECTS))

CXXFLAGS += -I/usr/local/include \

LDFLAGS = -L/usr/lib/gcc/x86_64-linux-gnu/4.8 \
		  -L/usr/lib/x86_64-linux-gnu \
		  -L/usr/lib \
		  -L/usr/local/lib \

CXX = g++

CXXFLAGS += -std=c++14 -Wall -Wno-reorder -fpermissive

LDLIBS 	+= -lavcodec -lavdevice -lavformat -lavutil -lswscale -lpthread -lrt

ifeq ($(DEBUG), 1)
	CXXFLAGS += -O0 -g3
else
	CXXFLAGS += -O2 -g0
endif

$(BDIR)/%.o : $(SDIR)/%.cpp $(DEPENDENCIES)
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(LDFLAGS) $(LDLIBS)

$(EXECUTABLE) : $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS) $(LDLIBS)

.PHONY : cleanobj clean

cleanobj :
	rm $(OBJECTS)

clean : cleanobj
	rm $(EXECUTABLE)