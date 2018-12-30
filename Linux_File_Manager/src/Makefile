ARCH            = x9
CROSS_COMPILE   =arm-linux-gnueabihf-
CC              = $(CROSS_COMPILE)gcc

CFLAGS  = -O2 -Wall
LIBS    = -lpthread -lsqlite3 -ldl
OBJECTS = main.o keyboard.o
CFLAGS  += -I/usr/local/$(ARCH)/include
LIBS    += -L/usr/local/$(ARCH)/lib
PROGRAM = test-assistant-$(ARCH)

all: $(PROGRAM)


%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(PROGRAM): $(OBJECTS)
	-$(RM) $@
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJECTS) $(LIBS)

clean:
	-$(RM) $(OBJECTS) $(PROGRA)
