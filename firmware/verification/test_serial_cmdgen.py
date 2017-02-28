# 
class FarmArea:
    def __init__(self):
        self.x0 = 1 #plant start x
        self.y0 = 1 #plant start y
        self.dx = 10 #plant offset distance x
        self.dy = 10 #plant offset distance x
        self.x_num = 4 #plant count in X axis
        self.y_num = 5 #plant count in Y axis
    def gen_plant_travel_cmds(self, fix_z = -100):
        pos_list = []
        for y in range(0,self.y_num):
            for x in range(0,self.x_num):
                pos = [self.x0 + x * self.dx, self.y0 + y * self.dy, fix_z]
                pos_list.append(pos)
        return pos_list
    
class CmdGen:
    def __init__(self):
        self.scrs = [] #"cmd string list"
        pass
    def add_route(self, route, cmd_after=""):
        for r1 in route:
            [x,y,z] = r1
            cmd = "G00 X%i Y%i Z%i" %(x,y,z)
            self.scrs.append(cmd)
            if cmd_after !="":
                self.scrs.append(cmd_after)
    def gen_cmds_file(self,filename):
        with open(filename,'w') as file:
            file.write("F83\n")
            cmds = "\n".join(self.scrs)
            file.write(cmds)
            file.write("\nF83\n")
            file.close()
    def desc(self):
        for scr in self.scrs:
            print(scr)

GENMODE_DEFROUTE = 1
GENMODE_POINT_TRAVEL = 2
def main():
    cmdgen = CmdGen()
    genmode = GENMODE_POINT_TRAVEL
    if genmode == GENMODE_DEFROUTE: 
        #route list defined as list [ point1, point2, ... ] , point = [x,y,z] 
        def_route = [[10,10,10],[20,20,20],[30,30,30]]
        cmdgen.add_route(def_route)
    elif genmode == GENMODE_POINT_TRAVEL:
        farm_area = FarmArea()
        pos_list = farm_area.gen_plant_travel_cmds()
        cmdgen.add_route(pos_list, "F83")
        
    cmdgen.desc()
    cmdgen.gen_cmds_file('serial_gencmds.txt')
    
if __name__ == "__main__":
    main()


