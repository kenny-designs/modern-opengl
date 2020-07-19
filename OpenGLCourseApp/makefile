GLM=../includes/GLM
CC=g++
CFLAGS=-o main.out -lGL -lGLU -lglfw3 -lGLEW -lX11 \
			 -lXxf86vm -lXrandr -lpthread -lXi \
			 -ldl -lXinerama -lXcursor -I$(GLM)

# LIBS=-L../includes/GLEW/lib

opengl: main.cpp Mesh.cpp Shader.cpp
	$(CC) main.cpp Mesh.cpp Shader.cpp $(CFLAGS)
	./main.out

.PHONY: clean

clean:
	rm *.out
