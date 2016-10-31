import math

def getDistance(lat1_deg,lon1_deg,lat2_deg,lon2_deg):
	# using the haversine formula https://en.wikipedia.org/wiki/Haversine_formula
    lat1 = lat1_deg * (math.pi/180)
    lon1 = lon1_deg * (math.pi/180)
    lat2 = lat2_deg * (math.pi/180)
    lon2 = lon2_deg * (math.pi/180)
    lat_diff = abs(lat1 - lat2)
    lon_diff = abs(lon1 - lon2)
    distance = 2*k_earthRadius*math.asin(math.sqrt(pow(math.sin(lat_diff/2),2) + math.cos(lat1)*math.cos(lat2)*pow(math.sin(lon_diff/2),2)))
    return distance


k_earthRadius = float(6371008)
lats = [48.06000, 48.24900]
lons = [11.35800, 11.72400]
xScale = 1000/getDistance(lats[0],lons[0],lats[1],lons[0])
yScale = 1000/getDistance(lats[0],lons[0],lats[0],lons[1])
dimensions = 10
change = [(dimensions/2)*xScale,(dimensions/2)*yScale]

writeFile = open('poi_translated.tsv', 'w')
fileName = "poi.tsv"
with open(fileName) as f:
    for line in f:
        lineList = line.split()
        lat = float(lineList[2])
        lon = float(lineList[3])
        point = [xScale*getDistance(lats[0],lons[0],lat,lons[0]), yScale*getDistance(lats[0],lons[0],lats[0],lon)]

        outputList = [lineList[0],lineList[1]]     # id and uid
        outputList.append(str(point[0]-change[0])) # min X
        outputList.append(str(point[0]+change[0])) # max X
        outputList.append(str(point[1]-change[1])) # min Y
        outputList.append(str(point[1]+change[1])) # max Y
        writeFile.write('\t'.join(outputList)+'\n')
writeFile.close()
