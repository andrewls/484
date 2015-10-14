MPI_HOME        = /users/faculty/snell/mpich
MPI_INCL        = $(MPI_HOME)/include
MPI_LIB         = $(MPI_HOME)/lib

SRC   			= lab4.c
TARGET     		= lab4

CC         		= $(MPI_HOME)/bin/mpicc
CFLAGS			= -O -I$(MPI_INCL)
LFLAGS     		= -L$(MPI_LIB) -lm -lmpich

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -lm -o $(TARGET)

run: $(TARGET)
	./$(TARGET) -p4pg $(TARGET).cfg

clean:
		/bin/rm -f  *.o
