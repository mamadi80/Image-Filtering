CC = g++
CFLAGS = -std=c++11

MAIN_EXECUTABLE_FILE = ImageFilters.out
all:   $(MAIN_EXECUTABLE_FILE)

$(MAIN_EXECUTABLE_FILE): readImg.cpp
	$(CC) $(CFLAGS) readImg.cpp -o $(MAIN_EXECUTABLE_FILE)
.PHONY: clean
clean:
	rm -rf *.out 
	


