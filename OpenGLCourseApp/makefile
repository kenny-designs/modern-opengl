GLEW=../includes/GLEW/include
GLFW=../includes/GLFW/include
CC=g++
#CFLAGS=-I$(GLEW) -I$(GLFW)
CFLAGS=-o main.out -lGL -lGLU -lglfw3 -lGLEW -lX11 \
			 -lXxf86vm -lXrandr -lpthread -lXi \
			 -ldl -lXinerama -lXcursor

LIBS=-L../includes/GLEW/lib

opengl: main.cpp
	$(CC) main.cpp $(CFLAGS)
	./main.out

.PHONY: clean

clean:
	rm *.out
