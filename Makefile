
# hot:
	# gcc hotplate.c -O3 -fopenmp -o hotplate

lab2:
	gcc hotplate_pthreads.c -pthreads -O3 -o hotplate

lab2Linux:
	gcc hotplate_pthreads.c -lpthread -O3 -o hotplate

all:
	lab2Linux

clean:
	rm hotplate
