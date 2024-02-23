#!/usr/bin/env python3
import random
import threading
import time
from collections import deque


class MySemaphore:
    def __init__(self, n: int, max_n: int = -1):
        if n < 0:
            raise ValueError("n must be non-negative")
        self._n = n
        if max_n < 0:
            self._max_n = n
        elif max_n < n:
            raise ValueError("if given, max_n must be >= n")
        else:
            self._max_n = max_n
        self._cv = threading.Condition(threading.Lock())

    def p(self):
        with self._cv:
            self._n -= 1
            while self._n < 0:
                self._cv.wait()

    def v(self):
        with self._cv:
            self._n += 1
            if self._n > self._max_n:
                raise ValueError("exceeded max_n")
            if self._n <= 0:
                self._cv.notify_all()


def consumer(q: deque, full_sem: MySemaphore, empty_sem: MySemaphore):
    while True:
        full_sem.p()
        item = q.popleft()
        empty_sem.v()
        print(f"Consumer: got {item}")
        if item is None:
            return
        #time.sleep(random.random())


def producer(q: deque, full_sem: MySemaphore, empty_sem: MySemaphore):
    for value in range(1, 101):
        empty_sem.p()
        q.append(value)
        full_sem.v()
        print(f"Producer: sent {value}")
        #time.sleep(random.random() * 0.1)
    empty_sem.p()
    q.append(None)
    full_sem.v()
    print(f"Producer: done (sent None)")


def main():
    full_sem = MySemaphore(0, max_n=5)
    empty_sem = MySemaphore(5)
    q = deque()

    tp = threading.Thread(target=producer, args=(q, full_sem, empty_sem))
    tc = threading.Thread(target=consumer, args=(q, full_sem, empty_sem))

    tc.start()
    tp.start()

    tp.join()
    tc.join()


if __name__ == "__main__":
    main()

