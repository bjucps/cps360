import threading

class UnBrokenSemaphore:
    def __init__(self, n: int):
        self.n = n
        self.wakeups = 0
        self.lock = threading.Lock()
        self.cv = threading.Condition(self.lock)
    
    def wait(self):
        with self.cv:
            self.n -= 1
            if self.n < 0:
                while True:
                    self.cv.wait()
                    if self.wakeups > 0:
                        break
                self.wakeups -= 1

    def post(self):
        with self.cv:
            self.n += 1
            if self.n <= 0:
                self.wakeups += 1
                self.cv.notify()

full = UnBrokenSemaphore(0)
empty = UnBrokenSemaphore(5)

def producer_thread():
    while True:
        empty.wait()
        # produce a thing, enqueue it
        full.post()

def consumer_thread():
    while True:
        full.wait()
        # consume the thing
        empty.post()