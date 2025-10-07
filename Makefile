CC = gcc
CFLAGS = -Wall -std=c99 -Imodules
SRC = vola.c modules/menu.c modules/utils.c modules/depenses.c modules/charges.c
TARGET = vola

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^ 

clean:
	rm -f $(TARGET)

.PHONY: clean