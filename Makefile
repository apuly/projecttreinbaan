TARGET = trein

CC = cc
LIBS = -lm
CFLAGS = -Wall -Werror

RM = rm -f

SOURCES = */*.c driver/treinbaandriver/*.c
HEADERS = includes/*.h
OBJECTS = */*.o

$(TARGET): $(SOURCES)
	$(CC) -o $@ $(LIBS) $(CFLAGS)  $(SOURCES)

clean:
	$(RM) $(OBJECTS)
