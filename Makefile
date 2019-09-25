CC=clang++
CFLAGS = -Wall -c
SERVER_BIN_DIR= ./bin
SERVER_LIB_DIR= ./lib
SERVER_SRC_DIR= ./src
SERVER_TMP_DIR= ./tmp
BUILD_FLAGS = -Wall

TCC = clang++
TFLAGS = -lgtest -lpthread
TIFLAGS = -lgtest -lglfw -lGLEW -ldl -lm -lGL -lGLU -lX11 -lpthread -lcglm -lcollectc
ILIBS = -lglfw -lGLEW -ldl -lm -lGL -lGLU -lX11 -lpthread -lcglm -lcollectc
IFLAGS = -Wl,-rpath=./lib/sharedlibs -L./lib/sharedlibs
ILOADER_LIB = ./tmp/loadpng.o

TARGET = $(SERVER_BIN_DIR)/app
FILES = $(SERVER_TMP_DIR)/main.o \
$(SERVER_TMP_DIR)/Application.o \
$(SERVER_TMP_DIR)/Manager.o \
$(SERVER_TMP_DIR)/Shader.o \
$(SERVER_TMP_DIR)/Tile.o \
$(SERVER_TMP_DIR)/Paint.o \
$(SERVER_TMP_DIR)/imgui.o \
$(SERVER_TMP_DIR)/imgui_draw.o \
$(SERVER_TMP_DIR)/imgui_widgets.o \
$(SERVER_TMP_DIR)/imgui_impl_glfw.o \
$(SERVER_TMP_DIR)/imgui_impl_opengl3.o \
#$(ILOADER_LIB)


$(TARGET): clean dirs LOADER IMGUI $(FILES)
	$(CC) -o $(SERVER_BIN_DIR)/app $(FILES) $(IFLAGS) $(INTERFACE) $(ILOADER_LIB) $(ILIBS)

$(SERVER_TMP_DIR)/main.o: $(SERVER_SRC_DIR)/main.cpp
	$(CC) $(CFLAGS) -o $(SERVER_TMP_DIR)/main.o $(SERVER_SRC_DIR)/main.cpp

$(SERVER_TMP_DIR)/Application.o: $(SERVER_SRC_DIR)/Application.cpp $(SERVER_SRC_DIR)/Application.h
	$(CC) $(CFLAGS) -o $(SERVER_TMP_DIR)/Application.o $(SERVER_SRC_DIR)/Application.cpp

$(SERVER_TMP_DIR)/Manager.o: $(SERVER_SRC_DIR)/Manager.cpp $(SERVER_SRC_DIR)/Manager.h
	$(CC) $(CFLAGS) -o $(SERVER_TMP_DIR)/Manager.o $(SERVER_SRC_DIR)/Manager.cpp

$(SERVER_TMP_DIR)/Tile.o: $(SERVER_SRC_DIR)/Tile.cpp $(SERVER_SRC_DIR)/Tile.h
	$(CC) $(CFLAGS) -o $(SERVER_TMP_DIR)/Tile.o $(SERVER_SRC_DIR)/Tile.cpp

$(SERVER_TMP_DIR)/Shader.o: $(SERVER_SRC_DIR)/Shader.cpp $(SERVER_SRC_DIR)/Shader.h
	$(CC) $(CFLAGS) -o $(SERVER_TMP_DIR)/Shader.o $(SERVER_SRC_DIR)/Shader.cpp

$(SERVER_TMP_DIR)/Paint.o: $(SERVER_SRC_DIR)/Paint.cpp $(SERVER_SRC_DIR)/Paint.h
	$(CC) $(CFLAGS) -o $(SERVER_TMP_DIR)/Paint.o $(SERVER_SRC_DIR)/Paint.cpp

IMGUI:
	$(CC) $(CFLAGS) -o $(SERVER_TMP_DIR)/imgui.o $(SERVER_LIB_DIR)/includes/imgui.cpp
	$(CC) $(CFLAGS) -o $(SERVER_TMP_DIR)/imgui_draw.o $(SERVER_LIB_DIR)/includes/imgui_draw.cpp
	$(CC) $(CFLAGS) -o $(SERVER_TMP_DIR)/imgui_widgets.o $(SERVER_LIB_DIR)/includes/imgui_widgets.cpp
	$(CC) $(CFLAGS) -o $(SERVER_TMP_DIR)/imgui_impl_glfw.o $(SERVER_LIB_DIR)/includes/imgui_impl_glfw.cpp
	$(CC) $(CFLAGS) -o $(SERVER_TMP_DIR)/imgui_impl_opengl3.o $(SERVER_LIB_DIR)/includes/imgui_impl_opengl3.cpp

LOADER: dirs $(SERVER_LIB_DIR)/lodepng.c
	clang $(CFLAGS) -o $(ILOADER_LIB) $(SERVER_LIB_DIR)/lodepng.c

dirs:
	mkdir -p  $(SERVER_BIN_DIR) $(SERVER_TMP_DIR)
clean:
	rm -rf  $(SERVER_BIN_DIR) $(SERVER_TMP_DIR)
	rm -f ./test*
