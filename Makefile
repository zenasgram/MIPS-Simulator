# For simulator
CC = g++
CPPFLAGS = -W -Wall -std=c++11

simulator: bin arithmetic.o bitwise.o program_counter.o processor.o register_file.o instruction_memory.o simulator.o main.o
	$(CC) $(CPPFLAGS) bin/arithmetic.o bin/bitwise.o bin/program_counter.o bin/processor.o bin/register_file.o bin/simulator.o bin/instruction_memory.o bin/main.o -o bin/mips_simulator

bin\mips_simulator: bin arithmetic.o bitwise.o program_counter.o processor.o register_file.o instruction_memory.o simulator.o main.o
	$(CC) $(CPPFLAGS) bin/arithmetic.o bin/bitwise.o bin/program_counter.o bin/processor.o bin/register_file.o bin/simulator.o bin/instruction_memory.o bin/main.o -o bin/mips_simulator

# Create binary directory
bin:
	mkdir bin

main.o: src/main.cpp
	$(CC) $(CPPFLAGS) -c src/main.cpp -o bin/main.o

arithmetic.o: src/instructions/arithmetic.cpp src/instructions/arithmetic.hpp
	$(CC) $(CPPFLAGS) -c src/instructions/arithmetic.cpp -o bin/arithmetic.o

bitwise.o: src/instructions/bitwise.cpp src/instructions/bitwise.hpp
	$(CC) $(CPPFLAGS) -c src/instructions/bitwise.cpp -o bin/bitwise.o

program_counter.o: src/program_counter.cpp src/program_counter.hpp
	$(CC) $(CPPFLAGS) -c src/program_counter.cpp -o bin/program_counter.o

processor.o: src/processor.cpp src/processor.hpp
	$(CC) $(CPPFLAGS) -c src/processor.cpp -o bin/processor.o

register_file.o: src/register_file.cpp src/register_file.hpp
	$(CC) $(CPPFLAGS) -c src/register_file.cpp -o bin/register_file.o

simulator.o: src/simulator.cpp src/simulator.hpp
	$(CC) $(CPPFLAGS) -c src/simulator.cpp -o bin/simulator.o

instruction_memory.o: src/instruction_memory.cpp src/instruction_memory.hpp
	$(CC) $(CPPFLAGS) -c src/instruction_memory.cpp -o bin/instruction_memory.o

# Dummy for build testbench to conform to spec. Could do nothing
testbench: bin bin/mips_testbench
	chmod u+x bin/mips_testbench

bin/mips_testbench: src/mips_testbench	
	cp ./src/mips_testbench ./bin/mips_testbench
	
# Delete compiled files
clean:
	rm bin/*
