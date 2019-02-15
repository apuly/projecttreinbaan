TARGET = trein

CC = cc
LIBS = -lm
CFLAGS = -Wall -Werror

RM = rm -f

SOURCES = */*.c
HEADERS = includes/*.h
OBJECTS = */*.o

$(TARGET): $(SOURCES)
	$(RM) $@
	$(CC) -o $@ $(LIBS) $(CFLAGS)  $(SOURCES)

clean:
	$(RM) $(OBJECTS)
