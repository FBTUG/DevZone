import threading 

class Thread(threading.Thread):

    def __init__(self, target):
        threading.Thread.__init__(self)
        self.__target = target
        self.__paused = True  # start out paused
        self.__exit = False
        self.__state = threading.Condition()

    def run(self):
        print("thread running")
        self.resume()
        while self.__exit is False:
            with self.__state:
                if self.__paused:
                    self.__state.wait()
            self.__target()

    def resume(self):
        print("thread resume")
        with self.__state:
            self.__paused = False
            self.__state.notify()

    def stop(self):
        print("thread stop")
        with self.__state:
            if self.__paused is False:
                self.__paused = True

    def exit(self):
        self.__exit = True
