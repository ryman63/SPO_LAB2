
all: my_program_build

CFLAGS += -I$(PWD)/libantlr3c-3.4/include -I$(PWD)/libxml2-vc140-static-32_64.2.9.4.1/lib/native/include
LDFLAGS += -L$(PWD)/libantlr3c-3.4/.libs -L$(PWD)/libxml2-vc140-static-32_64.2.9.4.1

my_program_build:
	gcc -g -O0 -o ./app src/*.c \
	 $(CFLAGS) \
	 -Dsize_t=size_t \
	 $(LDFLAGS) \
	 -lantlr3c \
	 -l:libxml2.so.2.9.14

clean:
	[ -f ./app ] && rm ./app || echo 

rebuild: clean my_program_build