# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra

# File names
SOURCE = simple.c
OUTPUT = simple
# Default target
all: $(OUTPUT)

# Compile the program
$(OUTPUT): $(SOURCE)
	$(CC) $(CFLAGS) $< -o $@

# Run the program
run: $(OUTPUT)
	./$(OUTPUT)

# Clean build files
clean:
	rm -f $(OUTPUT)

# Clean and rebuild
rebuild: clean all

# Help target
help:
	@echo "Available targets:"
	@echo "  all     : Build the program (default)"
	@echo "  run     : Build and run the program"
	@echo "  clean   : Remove build files"
	@echo "  rebuild : Clean and rebuild"
	@echo "  help    : Show this help message"

.PHONY: all run clean rebuild help