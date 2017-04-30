CC=gcc
CFLAGS=-Wall --pedantic -Wno-deprecated-declarations -std=c11
LIBS=-lGL -lGLU -lglut -lm ./libSOIL.a

# Windows (cygwin)
ifeq ($(OS), "Windows_NT")
LDFLAGS = -lopengl32 -lglu32 -lglut32
endif

# OS X
ifeq "$(shell uname)" "Darwin"
LIBS = -framework Carbon -framework QuickTime -framework OpenGL -framework GLUT
endif

SOURCES=$(wildcard *.c)

frogger: $(SOURCES)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean
clean:
	@rm -f *.o frogger

