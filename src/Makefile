LIBS_FLAGS = -lpthread

build:
	gcc ./api.c ./main.c -o ./../dist/bridge $(LIBS_FLAGS)

test:
	gcc ./api.c ./tests.c -o ./../dist/tests $(LIBS_FLAGS) -lcriterion
