def analyse(file_path):
    n = 0
    time = [0]*9
    percent = [0]*9
    NAMES = ("Inp", "RB In", "RB", "LB In", "LB", "GL In", "GL", "R In", "R")

    print("Reading", file_path,"...")

    with open(file_path, "r") as f:
        for line in f:
            n += 1
            s = tuple(map(float, line[:-2].split(";")))
            total = sum(s)
            for i,e in enumerate(s):
                time[i] += e
                if total > 0:
                    percent[i] += e / total

    print("Finished reading data")

    print("Process\t\tTotal Sec\tPercent")
    for i in range(9):
        print("{}:\t\t{:.1f}\t\t{:.1%}".format(NAMES[i], time[i], percent[i]/n))

analyse("benchmark.txt")
# for i in range(1,8):
#     print("=====", i, "=====")
#     analyse("benchmark{}.txt".format(i))
#     print("\n\n")
