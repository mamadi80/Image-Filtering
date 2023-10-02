CC = g++
CFLAGS = -std=c++11

MAIN_EXECUTABLE_FILE = ImageFilters.out
all:   $(MAIN_EXECUTABLE_FILE)

$(MAIN_EXECUTABLE_FILE): trd.cpp
	$(CC) $(CFLAGS) -pthread trd.cpp -o $(MAIN_EXECUTABLE_FILE)
.PHONY: clean
clean:
	rm -rf *.out 
	


