CC = gcc
CFLAGS = -I.
OBJS = main.o


oss:$(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) -lm

.c.o:
	$(CC) $(CFLAGS) -c $<


clean:
	rm -f oss *.o *.output
