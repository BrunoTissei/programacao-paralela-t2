import sys
import math
import numpy as np
import matplotlib.pyplot as plt

input_file = open(sys.argv[1], "r")
p, n = map(int, input_file.readline().split())

b_times, s_times, t_times = [], [], []
b_ovrhead, s_ovrhead, t_ovrhead = [], [], []
b_speedup, s_speedup, t_speedup = [], [], []
b_effec, s_effec, t_effec = [], [], []
b_sigma, s_sigma, t_sigma = [], [], []

proc = [x for x in xrange(1, p + 1)]

for i in xrange(p):
    b_tot, s_tot, t_tot = 0, 0, 0
    b_aux, s_aux, t_aux = [], [], []

    for j in xrange(n):
        read, build, search = map(float, input_file.readline().split())
        b_tot += build
        s_tot += search
        t_tot += (read + build + search)

        b_aux.append(build)
        s_aux.append(search)
        t_aux.append(build + search + read)

    b_times.append(b_tot / n)
    s_times.append(s_tot / n)
    t_times.append(t_tot / n)

    b_sigma.append(0.0)
    s_sigma.append(0.0)
    t_sigma.append(0.0)
    for j in xrange(n):
        b_sigma[i] += (b_aux[j] - b_times[i])**2
        s_sigma[i] += (s_aux[j] - s_times[i])**2
        t_sigma[i] += (t_aux[j] - t_times[i])**2

    b_sigma[i] = math.sqrt(b_sigma[i] / n)
    s_sigma[i] = math.sqrt(s_sigma[i] / n)
    t_sigma[i] = math.sqrt(t_sigma[i] / n)


for i in xrange(p):
    b_ovrhead.append((i + 1) * b_times[i] - b_times[0])
    s_ovrhead.append((i + 1) * s_times[i] - s_times[0])
    t_ovrhead.append((i + 1) * t_times[i] - t_times[0])

    b_speedup.append(b_times[0] / b_times[i])
    s_speedup.append(s_times[0] / s_times[i])
    t_speedup.append(t_times[0] / t_times[i])

    b_effec.append(b_speedup[i] / (i + 1))
    s_effec.append(s_speedup[i] / (i + 1))
    t_effec.append(t_speedup[i] / (i + 1))


graphs = ["overhead", "speedup", "efficiency", "avg_time", "std_deviation"]
graphs_names = ["Overhead [ms]", "Speedup", "Efficiency", "Average Time [ms]", "Standard Deviation [ms]"]
b_lists = [b_ovrhead, b_speedup, b_effec, b_times, b_sigma]
s_lists = [s_ovrhead, s_speedup, s_effec, s_times, s_sigma]
t_lists = [t_ovrhead, t_speedup, t_effec, t_times, t_sigma]

weak = True

for i in xrange(len(graphs)):

    if weak:
        plt.plot([1, 2, 4], b_lists[i], label="Build", color='#F92B14', linewidth=2)
        plt.plot([1, 2, 4], s_lists[i], label="Search", color='#2C126B', linewidth=2)
        plt.plot([1, 2, 4], t_lists[i], label="Total", color='#00933D', linewidth=2)

        plt.xlabel('n', fontsize=15)
        plt.xticks([1, 2, 4])
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
