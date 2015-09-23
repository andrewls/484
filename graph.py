import matplotlib.pyplot as plot

def main():
    numThreads = [1,2,4,8,16,32]
    executionTimes = [7.928031,4.068973,3.043015,2.584986,3.221051,3.594792]
    executionTimes2 = [8.407208,6.463637,2.934534,1.970580,2.332974,3.290185]
    executionTimes3 = [6.020465,3.192295,2.631030,2.003227,1.512740,2.720293]

    plot.plot(numThreads, executionTimes)
    plot.ylim([1,10])
    plot.title("Naive Implementation")
    plot.xlabel("Number of Threads")
    plot.ylabel("Execution Time (s)")
    plot.savefig("graph1.png")
    plot.close()

    plot.plot(numThreads, executionTimes2)
    plot.ylim([1,10])
    plot.title("Increased Parallelization")
    plot.xlabel("Number of Threads")
    plot.ylabel("Execution Time (s)")
    plot.savefig("graph2.png")
    plot.close()

    plot.plot(numThreads, executionTimes3)
    plot.ylim([1,10])
    plot.title("Increased Parallelization with Steady State Tracking")
    plot.xlabel("Number of Threads")
    plot.ylabel("Execution Time (s)")
    plot.savefig("graph3.png")
    plot.close()

if __name__ == '__main__':
    main()
