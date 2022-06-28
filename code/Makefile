target = a.out
src = $(wildcard *.cpp)
obj = $(patsubst %.cpp, %.o, $(src))

CC = g++
CPPFLAGS = -I./

$(target): $(obj)
	$(CC) -o $@ $^

%.o: %.cpp
	$(CC) -o $@ -c $^ $(CPPFLAGS)

.PHONY: clean
clean:
	rm $(obj)
