 CC      = gcc
 RM      = rm -f
 CFLAGS  = -O2 -Wall 

 OBJECTS = test.o clock.o  graphics.o  
 LIBS    = -lSDL2 -lGLESv2 -lavcodec -lavutil -lavformat -lswscale 
 PROGRAM = test_ffmpeg
 
 %.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(PROGRAM): $(OBJECTS)
	-$(RM) $@
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJECTS) $(LIBS)

clean:
	-$(RM) $(OBJECTS) $(PROGRA)