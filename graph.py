import matplotlib.pyplot as plot

def main():
    numNodes = [1,2,4,8,16,32]
    execution_time_vector_size_1 = [0.0018, 2.0546, 4.0151, 11.1284, 19.1411, 23.115244]
    execution_time_vector_size_65k = [0.0021, 0.1007, 0.1979, 0.3371, 0.4710, 0.6218]

    plot.plot(numNodes, execution_time_vector_size_1)
    plot.title("Time to reduce and broadcast a vector of size 1 10,000 times")
    plot.xlabel("Number of nodes")
    plot.ylabel("Execution Time (s)")
    plot.savefig("vector-size-1.png")
    plot.close()

    plot.plot(numNodes, execution_time_vector_size_65k)
    plot.title("Time to reduce and broadcast a vector of size 65k 1 time")
    plot.xlabel("Number of nodes")
    plot.ylabel("Execution time (s)")
    plot.savefig("vector-size-65k.png")
    plot.close()

if __name__ == '__main__':
    main()
