GLM=../includes/GLM
CC=g++
CFLAGS=-o main.out -lGL -lGLU -lglfw3 -lGLEW -lX11 \
			 -lXxf86vm -lXrandr -lpthread -lXi \
			 -ldl -lXinerama -lXcursor -I$(GLM)
CPP=main.cpp \
		Mesh.cpp \
		Shader.cpp \
		Window.cpp \
		Camera.cpp \
		Texture.cpp \
		Light.cpp \
		Material.cpp \
		DirectionalLight.cpp \
		PointLight.cpp \
		SpotLight.cpp

# LIBS=-L../includes/GLEW/lib

opengl: $(CPP)
	$(CC) $(CPP) $(CFLAGS)
	./main.out

.PHONY: clean

clean:
	rm *.out
