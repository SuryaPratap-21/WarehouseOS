CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -pthread
TARGET = warehouse_simulator
SRCDIR = src
DATADIR = data
OUTPUTDIR = output

SOURCES = $(SRCDIR)/main.cpp
HEADERS = $(SRCDIR)/*.h

all: dirs $(TARGET)

dirs:
	@mkdir -p $(DATADIR)
	@mkdir -p $(OUTPUTDIR)

$(TARGET): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES)
	@echo "Build successful: $(TARGET)"

run: $(TARGET)
	@./$(TARGET)

clean:
	@rm -f $(TARGET)
	@echo "Cleaned executable"

clean-data:
	@rm -rf $(DATADIR)/* $(OUTPUTDIR)/*
	@echo "Cleaned generated data and output files"

clean-all: clean clean-data
	@echo "Full cleanup complete"

.PHONY: all dirs run clean clean-data clean-all
