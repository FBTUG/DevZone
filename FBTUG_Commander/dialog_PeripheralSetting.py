import Tkinter
import tkMessageBox
import tkFont
#from Tkinter import *
import tkSimpleDialog

class PeripheralSetting(tkSimpleDialog.Dialog):
    # ########################################
    def __init__(self, master, arg_PinList=[('',0)]):
        print 'init'
        strFont= 'Arial'
        self.__myfont12 = tkFont.Font(family=strFont, size=12)
        self.__myfont12_Bold = tkFont.Font(family=strFont, size=12, weight= tkFont.BOLD)
        self.__myfont10 = tkFont.Font(family=strFont, size=10)
        self.__myfont10_Bold = tkFont.Font(family=strFont, size=10, weight= tkFont.BOLD)
        self.__PinList= arg_PinList
        self.__MaxRow= 7
        self.__CurrentRow= len(arg_PinList)
        self.__CurGridRow= self.__CurrentRow
        self.__NumberList= range(0, self.__MaxRow+1)
        self.__entries_Func= [0]
        self.__entries_PinNumb= [0]
        self.__btns_clear=[0]
        #self.master= master
    	tkSimpleDialog.Dialog.__init__(self, master, "Peripherals")
    # ########################################


    def body(self, master):
        print 'body of Dialog PERIPHERALS'
        Tkinter.Label(master, text="Function", font= self.__myfont12_Bold).grid(row=0, column=0)
        Tkinter.Label(master, text="Pin Number", font= self.__myfont12_Bold).grid(row=0, column=1)
        Tkinter.Button(master, text= '+', font= self.__myfont12_Bold, command= self.btn_add_click, fg='white',activeforeground= 'white', bg= '#007700', activebackground= '#00aa00').grid(row=0,column=2)

        for i in self.__NumberList:
            if i==0:
                continue
            en_func = Tkinter.Entry(master)
            #en_func.insert(Tkinter.END, '{0}'.format(i))
            self.__entries_Func.append(en_func)
            en_pinnumb= Tkinter.Entry(master)
            self.__entries_PinNumb.append(en_pinnumb)

            btn= Tkinter.Button(master, text= '-', font= self.__myfont12_Bold, command= lambda i=i: self.btn_clear_click(i),fg='white',activeforeground= 'white', bg= '#aa0000', activebackground= '#ee0000')
            self.__btns_clear.append(btn)
            '''
            en_func.grid(row=i+1,column=0)
            en_pinnumb.grid(row=i+1, column=1)
            btn.grid(row=i+1,column=2)
            '''
            if i <=  self.__CurrentRow:
                en_func.insert(Tkinter.END, self.__PinList[i-1][0])
                #en_func.insert(Tkinter.END, '{0}'.format(i))
                en_pinnumb.insert(Tkinter.END, self.__PinList[i-1][1])
                #'''
                en_func.grid(row=i,column=0)
                en_pinnumb.grid(row=i, column=1)
                btn.grid(row=i,column=2)
                '''
                en_func.grid_remove()
                en_pinnumb.grid_remove()
                btn.grid_remove()
                #'''
            #self.add_Row( i)
        return self.__entries_Func[0] # initial focus

    def apply(self):
        try:
            self.result=[]
            for i in range(1, len(self.__entries_Func)):
                r1, r2= self.__entries_Func[i].get(), self.__entries_PinNumb[i].get()
                if r1 != '' and r2 != '':
                    self.result.append([r1,int(r2)])
            #print 'result:', self.result
            print 'End of dialog' # or something
        except ValueError:
            tkMessageBox.showwarning("Bad input","Illegal values, please try again")

    def btn_clear_click(self, arg_index):
        clear_row= self.__NumberList.index(arg_index)
        '''
        print '============= CLEAR ============'
        print 'Clear Row:', clear_row
        print 'NumberLIst:', self.__NumberList
        print 'clear_index', arg_index
        gridInfo= self.__entries_Func[arg_index].grid_info()
        #print gridInfo
        print 'Clear Grid Row', gridInfo['row']
        #'''
        #'''
        self.__entries_Func[arg_index].delete(0, 'end')
        self.__entries_PinNumb[arg_index].delete(0, 'end')
        self.__entries_Func[arg_index].grid_forget()
        self.__entries_PinNumb[arg_index].grid_forget()
        self.__btns_clear[arg_index].grid_forget()
        '''
        self.__entries_Func[arg_index].grid_remove()
        self.__entries_PinNumb[arg_index].grid_remove()
        self.__btns_clear[arg_index].grid_remove()
        #'''
        tmp= self.__NumberList[clear_row]
        del self.__NumberList[clear_row]
        self.__NumberList.append(tmp)
        self.__CurrentRow= self.__CurrentRow-1
        #print '__CurrentRow:', self.__CurrentRow
        #'''
    def btn_add_click(self):
        '''
        print '============= ADD ============'
        print '### Current Row', self.__CurrentRow
        print 'NumberLIst:', self.__NumberList
        for i in range(1,len(self.__entries_Func)):
            tmp= self.__NumberList[i]
            gridInfo= self.__entries_Func[tmp].grid_info()
            if len(gridInfo)!=0:
                print 'Row ',str(i),' Entries List[', str(tmp),']: ', self.__entries_Func[tmp].grid_info()['row']
            else:
                print 'Row ',str(i),' empty'
        #'''
        if self.__CurrentRow < self.__MaxRow:
            self.__CurrentRow= self.__CurrentRow+1
            self.__CurGridRow= self.__CurGridRow+1
            #self.__CurGridRow= self.__CurrentRow
            add_index= self.__NumberList[self.__CurrentRow]
            '''
            print 'Added Row:', self.__CurrentRow
            print 'add_index (NumberList[{0}]): {1}'.format(self.__CurrentRow,add_index)
            print 'Grid Row:', self.__CurGridRow
            #'''
            self.__entries_Func[add_index].grid(row=self.__CurGridRow, column=0)
            #self.__entries_Func[add_index].delete(0, 'end')
            self.__entries_PinNumb[add_index].grid(row=self.__CurGridRow, column=1)
            #self.__entries_PinNumb[add_index].delete(0, 'end')
            self.__btns_clear[add_index].grid(row=self.__CurGridRow, column=2)
            #print 'Row ',str(self.__CurrentRow),' Entries List[', str(add_index),']: ', self.__entries_Func[add_index].grid_info()['row']
        elif self.__CurrentRow== self.__MaxRow:
            print 'Max of Row is ', self.__MaxRow
