CC = gcc
CFLAGS = -g
OBJS = main.o
OBJS2 = user.o


all: oss user
oss:$(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) 

user:$(OBJS2)
	$(CC) $(CFLAGS) -o $@ $(OBJS2) 



.c.o:
	$(CC) $(CFLAGS) -c $<

user.o: user.c
	$(CC) $(CFLAGS) -c user.c

clean:
	rm -f user oss *.o *.output
