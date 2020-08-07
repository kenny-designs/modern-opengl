GLM=../includes/GLM
ASSIMP=../includes/ASSIMP/include
CC=g++
CFLAGS=-o main.out -lGL -lGLU -lglfw3 -lGLEW -lX11 \
			 -lXxf86vm -lXrandr -lpthread -lXi \
			 -ldl -lXinerama -lXcursor -lassimp \
			 -I$(GLM) -I$(ASSIMP)

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
		SpotLight.cpp \
		Model.cpp


opengl: $(CPP)
	$(CC) $(CPP) $(CFLAGS)
	./main.out

.PHONY: clean

clean:
	rm *.out
