heightmap = []
basinmap = []
basinsizes = []

def valid(x,y):
    return x >=0 and x < len(heightmap) and y >=0 and y< len(heightmap[x])
def valid2(x,y):
    return valid(x,y) and heightmap[x][y] != '9' and not basinmap[x][y]

def dfs(x,y):
    currsize = 1
    queue = []
    queue.append((x,y))
    basinmap[x][y] = True
    
    while(not len(queue) == 0):
        coords = queue.pop()
        i = coords[0]
        j = coords[1]
        #print(str(i) + ', ' + str(j))
        if valid2(i-1,j):
            queue.append((i-1,j))
            basinmap[i-1][j] = True
            currsize += 1
        if valid2(i+1,j):
            queue.append((i+1,j))
            basinmap[i+1][j] = True
            currsize += 1
        if(valid2(i,j-1)):
            queue.append((i,j-1))
            basinmap[i][j-1] = True
            currsize += 1
        if(valid2(i,j+1)):
            queue.append((i,j+1))
            basinmap[i][j+1] = True
            currsize += 1
    #print('appending: ' + str(currsize))
    basinsizes.append(currsize)

def basins():   
    global basinmap
    basinmap = [[False for j in range(len(heightmap[i]))] for i in range(len(heightmap))]
    
    for i in range(len(heightmap)):
        for j in range(len(heightmap[i])):
            if(valid2(i,j)):
                dfs(i,j)
    basinsizes.sort()
    return (basinsizes[-1],basinsizes[-2],basinsizes[-3])

def main():
  print(chr(98))
  f = open('input9.txt','r')
  global heightmap
  heightmap = [line[:-1] for line in f]
  risksum = 0

  for i in range(len(heightmap)):
    for j in range(len(heightmap[i])):
        currval = int(heightmap[i][j])
        if (not valid(i-1,j) or int(heightmap[i-1][j]) > currval) and (not valid(i+1,j) or int(heightmap[i+1][j]) > currval) and (not valid (i,j-1) or int(heightmap[i][j-1]) > currval) and (not valid(i,j+1) or int(heightmap[i][j+1]) > currval):
            risksum += (currval+1)
  print(risksum)
  print(basins())

if __name__ == "__main__":
    main()
