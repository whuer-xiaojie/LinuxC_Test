# arm-linux-gnueabihf-gcc debug.c main.c -o log-test zlog-x9.a -lpthread
CC      =arm-linux-gnueabihf-gcc
RM      = rm -f
CFLAGS  =  -O2 -Wall
LIBS    = -lpthread zlog-x9.a
OBJECTS = debug.o main.o

PROGRAM = log_test-x9

all: $(PROGRAM)


%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(PROGRAM): $(OBJECTS)
	-$(RM) $@
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJECTS) $(LIBS)

clean:
	-$(RM) $(OBJECTS) $(PROGRA)