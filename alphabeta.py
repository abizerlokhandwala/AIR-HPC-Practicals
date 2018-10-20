import random
import queue as q

INF = 10**18
global ptr
ptr = 0
arr = [-1,3,5,10,-4,-6,-10,-10]

class Node:
    def __init__(self,val,num_child):
        self.val = val
        self.num_child = num_child
        self.child = [None]*num_child

def random_branch():
    # return random.randint(1,4)
    return 2

def alphabeta(curr_player,depth,alpha,beta):
    if depth>=3:
        # val = random.randint(-100,100)
        global ptr
        val = arr[ptr]
        ptr+=1
        return (Node(val,0))

    if curr_player == 0:
        best = -INF
        num_child = random_branch()
        node = Node(best,num_child)
        for i in range(num_child):
            node_child = alphabeta(curr_player^1,depth+1,alpha,beta)
            node.child[i] = node_child
            best = max(best,node_child.val)
            alpha = max(best,alpha)
            # print("Player: "+str(curr_player)+" Value: "+str(node_child.val)+" Best: "+str(best)+" Alpha: "+str(alpha)+" Beta: "+str(beta))
            if beta<=alpha:
                ptr+=(2**(3-depth)-1)
                # print("Breaking")
                break

        node.val = best
        return node
    else:
        best = INF
        num_child = random_branch()
        node = Node(best,num_child)
        for i in range(num_child):
            node_child = alphabeta(curr_player^1,depth+1,alpha,beta)
            node.child[i] = node_child
            best = min(best,node_child.val)
            beta = min(best,beta)
            # print("Player: "+str(curr_player)+" Value: "+str(node_child.val)+" Best: "+str(best)+" Alpha: "+str(alpha)+" Beta: "+str(beta))
            if beta<=alpha:
                ptr+=(2**(3-depth)-1)
                # print("Breaking")
                break

        node.val = best
        return node

def bfs(root):
    qq = q.Queue()
    depth = 0
    qq.put((root,depth))
    while qq.empty()==False:
        node = qq.get()
        print("Depth: "+str(node[1])+" Value: "+str(node[0].val))
        for i in range(node[0].num_child):
            if node[0].child[i] is not None:
                qq.put((node[0].child[i],node[1]+1))

root = alphabeta(0,0,-INF,INF)
bfs(root)
