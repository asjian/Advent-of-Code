import random
visited = []

def visitedcheck(map,i,j,length):
    global visited

    if not visited[i][j][0]:
        visited[i][j] = (True,length)
        return False
    else:
        if length < visited[i][j][1]:
            visited[i][j] = (True,length)
            return False
        else:
            return True
        
def inbounds(map,x,y,length): #1 means wall, -1 means visited
    return x >= 0 and x<len(map) and y >=0 and y < len(map[x]) and not visitedcheck(map,x,y,length+map[x][y])

def legalmoves(map,x,y,length):
    moves = []

    if inbounds(map,x,y+1,length):
        moves.append((x,y+1,length+map[x][y+1]))

    if inbounds(map,x+1,y,length):
        moves.append((x+1,y,length+map[x+1][y]))

    if inbounds(map,x,y-1,length):
        moves.append((x,y-1,length+map[x][y-1]))

    if inbounds(map,x-1,y,length):
        moves.append((x-1,y,length+map[x-1][y]))

    return moves

def cost(position,xf,yf):
    return position[2] + (xf - position[0]) + (yf - position[1])

def bestindex(positions,xf,yf):
    if(len(positions) == 0):
        return -1

    mincost = 9000 * 9000
    minindex = 0
    for i in range(0,len(positions)):
        if cost(positions[i],xf,yf) < mincost:
            minindex = i
            mincost = cost(positions[i],xf,yf)
    
    return minindex

def solution(map):
    global visited
    xf = len(map) - 1
    yf = len(map[xf]) - 1

    visited = [[(False,(9000)*(9000)) for j in range(len(map[i]))] for i in range(len(map))]

    positions = []
    positions.append((0,0,0)) #i,j,path length to remove wall
    visited[0][0] = (True, 0)

    while(positions):
        popindex = bestindex(positions,xf,yf)
        info = positions.pop(popindex)
        i = info[0]
        j = info[1]
        length = info[2]
        
        if i == xf and j == yf:
            return length
            
        for move in legalmoves(map,i,j,length):
            positions.append(move)
        
    return -1

def main():
    f = open('input15.txt','r')
    grid = [[int(char) for char in line.strip()] for line in f]
    print(solution(grid))

if __name__ == "__main__":
    main()