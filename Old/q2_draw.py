from matplotlib import pyplot as plt
from matplotlib.patches import Rectangle
import pylab

def draw(array,i,style):
    r = array[i]
    x = r[0][0] # left
    y = r[1][1] # bottom
    """
    if x > bigX:
        bigX = x
    if y > bigY:
        bigY = y
    """
    width =  r[1][0] - x  # right - left
    height = r[0][1] - y # top - bottom
    print height
    currentAxis.add_patch(Rectangle((x, y), width, height, fill=None, alpha=1,linestyle=style))
    if style == 'solid':
        currentAxis.annotate(str(i), (x + width/2, y + height/2), color='black', weight='bold', fontsize=12, ha='center', va='center')
#    else:
#        currentAxis.annotate(str(i+len(points)), (x + width/2, y + height/2), color='black', weight='bold', fontsize=12, ha='center', va='center')
points = [[(2,25),(5,23)], [(3,20),(7,17)], [(1,13),(4,11)], [(1,3),(4,0)], [(6,24),(9,21)], [(7,20),(9,15)],
 [(6,8),(13,3)], [(17,22),(20,9)], [(19,12),(24,9)], [(19,8),(23,6)], [(21,25),(26,21)],
 [(20,17),(30,15)], [(25,16),(28,12)], [(13,22),(17,19)]]

#dotted = [[(1,13),(13,0)],[(2,25),(9,15)],[(13,22),(24,6)],[(20,25),(30,12)]]
dotted = [[(1,13),(13,0)],[(2,25),(9,15)],[(7,25),(30,15)],[(17,22),(28,6)]]

plt.figure()
currentAxis = plt.gca()
#bigY = 0
#bigX = 0
for i in range(len(points)):
    draw(points,i,'solid')
for i in range(len(dotted)):
    draw(dotted,i,'dashed')
pylab.xlim([-1,31])
pylab.ylim([-1,26])
plt.show()
