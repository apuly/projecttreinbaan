TARGET = trein

CC = cc
LIBS = -lm
CFLAGS = -Wall -Werror

RM = rm -f

SOURCES = */*.c driver/treinbaandriver/*.c driver/sensordriver/DriverSpawner.c
HEADERS = includes/*.h
OBJECTS = */*.o

$(TARGET): $(SOURCES)
	$(CC) -o $@ $(LIBS) $(CFLAGS)  $(SOURCES)

clean:
	$(RM) $(OBJECTS)

        
sensdriver:
	cc -o sensdriver -Wall -Werror driver/sensordriver/linkedlist.c driver/sensordriver/sensordriver.c

