from __future__ import print_function
import subprocess
import sys

ITERATIONS = 10

def main():
	maxworkers = 16
	maxcounter = 10000000

	try:
		if (len(sys.argv) == 3):
			maxworkers = int(sys.argv[1])
			maxwcounter = int(sys.argv[2])
		elif (len(sys.argv) == 2):
			maxworkers = int(sys.argv[1])
		elif (len(sys.argv) == 1):
			pass
		else:
			raise
	except:
		print("Usage: tests.py [maxworkers] [maxcounter]")
		return

	print("count: {}".format(maxcounter))
	print("workers\ttime")

	i = 1
	while i <= maxworkers:
		t = run_test(maxcounter = maxcounter, maxworkers = i, iterations = ITERATIONS)
		print("{}\t{}".format(i, t))
		i *= 2

def run_test(**kwargs):
	iterations = kwargs["iterations"]
	m = kwargs["maxcounter"]
	w = kwargs["maxworkers"]
	
	tot = 0
	for i in range(0, iterations):
		out = subprocess.check_output("(time -f %e -- ./lab1 -m {} -w {}) 2>&1".format(m, w), shell=True)
		tot += float(out)
	return tot / iterations

main()