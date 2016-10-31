def resize(lst):
    rec = points[lst[0]]
    right = rec[1][0]
    left = rec[0][0]
    top = rec[0][1]
    bottom = rec[1][1]
    for i in range(0,len(lst)):
        rec = points[lst[i]]
        if right < rec[1][0]: right = rec[1][0]
        if left > rec[0][0]: left = rec[0][0]
        if top < rec[0][1]: top = rec[0][1]
        if bottom > rec[1][1]: bottom = rec[1][1]
    return [(left,top),(right,bottom)]

def getD(rec1, rec2):
    right = rec1[1][0]
    left = rec1[0][0]
    top = rec1[0][1]
    bottom = rec1[1][1]
    if right < rec2[1][0]: right = rec2[1][0]
    if left > rec2[0][0]: left = rec2[0][0]
    if top < rec2[0][1]: top = rec2[0][1]
    if bottom > rec2[1][1]: bottom = rec2[1][1]

    e1 = (rec1[1][0] - rec1[0][0]) * (rec1[0][1] - rec1[1][1])
    e2 = (rec2[1][0] - rec2[0][0]) * (rec2[0][1] - rec2[1][1])
    j = (right-left) * (top-bottom)
    d = j - e1 - e2
    return d

def pickSeeds():
    bigD = 0
    mostWasteful = [0,0]
    for n1 in unusedPoints:
        for n2 in unusedPoints:
            if n1 >= n2: continue
            d = getD(points[n1],points[n2])
            if d > bigD:
                bigD = d
                mostWasteful = [n1,n2]
            #    bigJ = [(left,top),(right,bottom)]
    #print "Most Wasteful:", points[mostWasteful[0]], points[mostWasteful[1]]
    return mostWasteful

def findNext(mostWasteful):
    rtree = [[mostWasteful[0]],[mostWasteful[1]]]
    newSize = [points[mostWasteful[0]],points[mostWasteful[1]]]
    del(unusedPoints[unusedPoints.index(mostWasteful[0])])
    del(unusedPoints[unusedPoints.index(mostWasteful[1])])
    length = len(unusedPoints)
    for _ in range(length):
        biggestDifference = 0
        for i in range(len(unusedPoints)):
            d1 = getD(newSize[0],points[unusedPoints[i]])
            d2 = getD(newSize[1],points[unusedPoints[i]])
            difference = abs(d1-d2)
            if biggestDifference < difference:
                biggestDifference = difference
                if d1 < d2:
                    pair = [0,unusedPoints[i]]
                else:
                    pair = [1,unusedPoints[i]]
        rtree[pair[0]].append(pair[1])
        newSize[pair[0]] = resize(rtree[pair[0]])
        print rtree
        del(unusedPoints[unusedPoints.index(pair[1])])
    return rtree

"""
points = {0:[(2,25),(5,23)], 1:[(3,20),(7,17)], 2:[(1,13),(4,11)], 3:[(1,3),(4,0)],
          4:[(6,24),(9,21)], 5:[(7,20),(9,15)], 6:[(6,8),(13,3)], 7:[(17,22),(20,9)],
          8:[(19,12),(24,9)], 9:[(19,8),(23,6)], 10:[(21,25),(26,21)], 11:[(20,17),(30,15)],
          12:[(25,16),(28,12)], 13:[(13,22),(17,19)]}
unusedPoints = [0,1,2,3,4,5,6,7,8,9,10,11,12,13]
"""

points = {0:[(2,25),(5,23)], 1:[(3,20),(7,17)], 2:[(1,13),(4,11)], 3:[(1,3),(4,0)],
          4:[(6,24),(9,21)], 5:[(7,20),(9,15)], 6:[(6,8),(13,3)], 7:[(17,22),(20,9)],
          8:[(19,12),(24,9)], 9:[(19,8),(23,6)], 10:[(21,25),(26,21)], 11:[(20,17),(30,15)],
          12:[(25,16),(28,12)], 13:[(13,22),(17,19)]}
"""
points = {0:[(2,25),(9,15)],2:[(1,13),(13,0)], 5:[(7,25),(30,15)],7:[(7,22),(24,9)],
          8:[(19,12),(24,9)], 9:[(17,22),(28,6)], 10:[(21,25),(26,21)], 11:[(20,17),(30,15)],
          12:[(25,16),(28,12)], 13:[(13,22),(17,19)]}
unusedPoints = [5,9,13]
"""
unusedPoints = [5,7,8,9,10]

mostWasteful = pickSeeds()
rtree = findNext(mostWasteful)
#rtree = findNext([5,9])
print rtree

dotted = []
for lst in rtree:
    dotted.append(resize(lst))
print dotted
