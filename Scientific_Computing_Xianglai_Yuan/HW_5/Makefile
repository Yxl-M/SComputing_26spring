CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -O2
LDLIBS = -lm

TARGET = legendre_stability
SRC = src/legendre_stability.c
C_RESULTS = results/legendre_errors_c.csv

.PHONY: all run python clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $< $(LDLIBS) -o $@

run: $(TARGET)
	mkdir -p results
	./$(TARGET) 50 80 $(C_RESULTS)

python:
	python3 scripts/legendre_mpmath_analysis.py

clean:
	rm -f $(TARGET)
	rm -f results/*.csv
	rm -f plots/*.png
