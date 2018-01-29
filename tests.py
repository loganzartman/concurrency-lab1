from __future__ import print_function
import subprocess
import sys

def main():
	iterations = 4
	maxworkers = 16
	maxcounter = 10000000
	write_prob = 0.5
	
	try:
		if (len(sys.argv) == 4):
			maxworkers = int(sys.argv[1])
			maxwcounter = int(sys.argv[2])
			write_prob = float(sys.argv[3])
		elif (len(sys.argv) == 1):
			pass
		else:
			raise
	except:
		print("Usage: tests.py [maxworkers] [maxcounter] [write-probability]")
		return

	test_time(maxcounter = maxcounter, maxworkers = maxworkers, write_prob = write_prob,
		iterations = iterations)
	test_stats(maxcounter = maxcounter, maxworkers = maxworkers, write_prob = write_prob,
		iterations = iterations)

def test_time(maxcounter, maxworkers, write_prob, iterations):
	print("count: {}".format(maxcounter))
	print("workers,time")

	i = 1
	while i <= maxworkers:
		t = run_test_time(maxcounter = maxcounter, workers = i, write_prob = write_prob,
			iterations = iterations)
		print("{},{}".format(i, t))
		i *= 2

def test_stats(maxcounter, maxworkers, write_prob, iterations):
	print("count: {}".format(maxcounter))
	print("workers,ratio,imbalance")

	i = 1
	while i <= maxworkers:
		(ratio, imbalance) = run_test_stats(maxcounter = maxcounter, workers = i, 
			write_prob = write_prob, iterations = iterations)
		print("{},{},{}".format(i, ratio, imbalance))
		i *= 2

def run_test_stats(maxcounter, workers, write_prob, iterations):
	avgRatio = 0
	avgImbalance = 0

	for i in range(0, iterations):
		out = subprocess.check_output("./lab1 -i -m {} -w {} -p {}"
			.format(maxcounter, workers, write_prob), shell=True)
		items = out.split(b",")
		avgRatio += float(items[0]) / iterations
		avgImbalance += float(items[1]) / iterations
	return (avgRatio, avgImbalance)


def run_test_time(maxcounter, workers, write_prob, iterations):
	tot = 0
	for i in range(0, iterations):
		out = subprocess.check_output(
			"(time -f %e -- ./lab1 -m {} -w {} -p {}) 2>&1"
				.format(maxcounter, workers, write_prob), shell = True)
		tot += float(out)
	return tot / iterations

main()