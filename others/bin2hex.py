f1 = open('in', 'r')
f2 = open('out', 'w')

for line in f1:
    f2.write('%08x\n' % (int(line[:32], 2)))

f1.close()
f2.close()