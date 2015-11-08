
all:
	gcc -I. -c string.c printf.c scanf.c -W -Wall -Wextra -Wno-unused-parameter

clean:
	rm *.o *~
