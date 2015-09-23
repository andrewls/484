
# hot:
	# gcc hotplate.c -O3 -fopenmp -o hotplate

lab2:
	gcc hotplate_pthreads.c -pthreads -O3 -o hotplate

all:
	lab2

clean:
	rm hotplate
