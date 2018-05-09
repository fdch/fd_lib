from itertools import combinations
lis = [1,2,3,4,5,6]
combs = []

def getcomb( arr ):
	"get an array and find combinations"
	for i in xrange(1, len(arr) + 1):
		combs.append(list(combinations(arr,i))
		

getcomb(lis)
print list(combs)
	# for f in combs:
	# 	r = set(lis) - set(f)
	# 	print list(r)
	# 	for j in xrange(1, len(r) + 1):
	# 		rcombs = combinations(r,j)
	# 		print list(rcombs)
	# 		for k in rcombs:
	# 			rr = set(r) - set(k)
	# 			print list(rr)
	# 			for l in xrange(1, len(r) + 1):
	# 				rrcombs = combinations(rr,l)
	# 				print list(rrcombs)
