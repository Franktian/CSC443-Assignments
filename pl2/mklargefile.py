import sys
import random

letters = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789'
# letters += "!@#$%^&*(()_+-=[]{}|abcdefghijkmnopqrstuvwxyz"
try:
    filename, n_records = sys.argv[1:3]
    n = int(n_records)
except:
    print 'Usage: %s <outfile> <number of records>' % sys.argv[0]
    sys.exit(0)

with open(filename, 'w') as f:
    for _ in xrange(n):
        line = ''.join([random.choice(letters) for j in range(7)])
        print >>f, line

print "Generated %d random records in %s." % (n, filename)        
