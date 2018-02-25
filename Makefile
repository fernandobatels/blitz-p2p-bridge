LIBS_FLAGS = -lpthread

build:
	gcc ./src/api.c ./src/main.c -o ./dist/bridge $(LIBS_FLAGS)
	gcc ./src/api.c ./src/tests.c -o ./dist/tests $(LIBS_FLAGS) -lcriterion
