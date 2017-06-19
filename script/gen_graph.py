import sys
import math
import numpy as np
import matplotlib.pyplot as plt

input_file = open(sys.argv[1], "r")
p, n = map(int, input_file.readline().split())

b_speedup, s_speedup, t_speedup = [], [], []
b_effec, s_effec, t_effec = [], [], []

proc = [x for x in xrange(1, p + 1)]

b_times = [ 0 for i in xrange(p) ]
s_times = [ 0 for i in xrange(p) ]
t_times = [ 0 for i in xrange(p) ]

for j in xrange(n):
    b_tot, s_tot, t_tot = 0, 0, 0

    for i in xrange(p):
        read, build, search, total = map(float, input_file.readline().split())
        b_times[i] += build
        s_times[i] += search
        t_times[i] += total

for i in xrange(p):
    b_times[i] /= n
    s_times[i] /= n
    t_times[i] /= n

#b_times.append(b_tot / n)
#s_times.append(s_tot / n)
#t_times.append(t_tot / n)


for i in xrange(p):
    b_speedup.append(b_times[0] / b_times[i])
    s_speedup.append(s_times[0] / s_times[i])
    t_speedup.append(t_times[0] / t_times[i])

    b_effec.append(b_speedup[i] / (i + 1))
    s_effec.append(s_speedup[i] / (i + 1))
    t_effec.append(t_speedup[i] / (i + 1))


graphs = ["speedup", "efficiency", "avg_time"]
graphs_names = ["Speedup", "Efficiency", "Average Time [ms]"]
b_lists = [b_speedup, b_effec, b_times]
s_lists = [s_speedup, s_effec, s_times]
t_lists = [t_speedup, t_effec, t_times]

weak = True

for i in xrange(len(graphs)):

    if weak:
        plt.plot([1, 2, 4, 8], b_lists[i], label="Build", color='#F92B14', linewidth=2)
        plt.plot([1, 2, 4, 8], s_lists[i], label="Search", color='#2C126B', linewidth=2)
        plt.plot([1, 2, 4, 8], t_lists[i], label="Total", color='#00933D', linewidth=2)

        plt.xlabel('n', fontsize=15)
        plt.xticks([1, 2, 4, 8])
        # plt.xticks(np.arange(min(proc), max(proc)+1, 1.0))
    else:
        plt.plot(proc, b_lists[i], label="Build", color='#F92B14', linewidth=2)
        plt.plot(proc, s_lists[i], label="Search", color='#2C126B', linewidth=2)
        plt.plot(proc, t_lists[i], label="Total", color='#00933D', linewidth=2)

        plt.xlabel('Threads', fontsize=15)

    plt.ylabel(graphs_names[i], fontsize=15)

    plt.legend(loc='best')

    plt.savefig(graphs[i])
    plt.gcf().clear()
