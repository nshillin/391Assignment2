import math

def newLatitude(lat,change):
    newLat = lat+(change/k_earthRadius)*(180/math.pi)
    return str(newLat)

def newLongitude(lon,lat,change):
    newLon = lon+((change/k_earthRadius)*(180/math.pi)/math.cos(lat * math.pi/180))
    return str(newLon)

k_earthRadius = float(6371008)
change = 10/2
writeFile = open('poi_edited.tsv', 'w')
fileName = "poi.tsv"
bigLat = float(0)
bigLon = float(0)
with open(fileName) as f:
    for line in f:
        lineList = line.split()
        lat = float(lineList[2])
        lon = float(lineList[3])
        if lat>bigLat:
            bigLat = lat
        if lon>bigLon:
            bigLon = lon
        outputList = [lineList[0],lineList[1]]
        outputList.append(newLatitude(lat,-change))
        outputList.append(newLatitude(lat,change))
        outputList.append(newLongitude(lon,lat,-change))
        outputList.append(newLongitude(lon,lat,change))
        writeFile.write('\t'.join(outputList)+'\n')
print bigLat, bigLon
writeFile.close()
