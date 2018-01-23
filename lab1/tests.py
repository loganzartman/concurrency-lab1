from __future__ import print_function
import subprocess
import sys

def main():
	maxworkers = 1
	maxcounter = 10000000

	try:
		if (len(sys.argv) == 3):
			maxworkers = int(sys.argv[1])
			maxwcounter = int(sys.argv[2])
		elif (len(sys.argv) == 2):
			maxworkers = int(sys.argv[1])
		else:
			raise
	except:
		print("Usage: tests.py maxworkers [maxcounter]")
		return

	i = 1
	while i <= maxworkers:
		run_test(maxcounter, i)
		i *= 2

def run_test(m, w):
	print("\nmaxcounter: {}\nworkers: {}".format(m, w))
	subprocess.call("time -f \"real: %es\" -- ./lab1 -m {} -w {}".format(m, w), shell=True)

main()