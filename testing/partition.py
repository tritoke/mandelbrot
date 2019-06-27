from math import floor
from functools import reduce
import operator

xlen = 10
ylen = 8
pixels = [[i for i in range(y*xlen, (y+1)*xlen)] for y in range(ylen)]
threads = 8

def part(threadnum):
    """
    returns the start and end rows of pixels for the thread to render
    thread 0 should have row 0 and thread threadnum-1 should have the last row.
    """
    start = floor(ylen * (threadnum / threads))
    end = floor(ylen * ((threadnum + 1) / threads))
    return start, end

partitions = []
for i in range(threads):
    s, e = part(i)
    partitions.append(set(reduce(operator.add, pixels[s:e])))

# check the partitions

all_pixels = set(reduce(operator.add, pixels))

for part in partitions:
    all_pixels -= part

assert(all_pixels == set())

for a,b in zip(partitions[:-1], partitions[1:]):
    assert(a.intersection(b) == set())

print("tests passed")
