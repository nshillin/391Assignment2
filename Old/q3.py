import re
f = open('q3points.txt', 'r')
text = f.read()
lines = text.split('\n')
for i in range(len(lines)):
    l = re.findall(r'\d+', lines[i]) # current order = minX, maxY, maxX, minY
    l = [l[0],l[2],l[3],l[1]]
    print "\t".join([str(i)] + l)
