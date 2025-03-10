#!/usr/bin/env python3
import random
import threading
import time
import sys
from collections import deque

ITEMS = ["trench coats", "pork pies", "race cars", "cheese pizzas", "hub caps", "ladder jacks", "pickle jars", "waffle irons"]


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
        self._wakeups = 0    # hat tip: The Little Book of Semaphores
        self._spurious_wakes = 0
        self._time_sleeping_ns = 0

    def report(self, name: str):
        print(f"Semaphore '{name}' [{id(self)}]: {self._time_sleeping_ns:,} ns sleeping, {self._spurious_wakes} spurwakes")

    def wait(self):
        with self._cv:
            self._n -= 1
            if self._n < 0:
                while True:
                    ns1 = time.process_time_ns()
                    self._cv.wait()
                    self._time_sleeping_ns += time.process_time_ns() - ns1
                    if self._wakeups >= 1:
                        break
                    else:
                        self._spurious_wakes += 1
                self._wakeups -= 1

    def post(self):
        with self._cv:
            self._n += 1
            if self._n > self._max_n:
                raise ValueError("exceeded max_n")
            if self._n <= 0:
                self._wakeups += 1
                self._cv.notify()


def consumer(i: int, q: deque, full_sem: MySemaphore, empty_sem: MySemaphore):
    while True:
        full_sem.wait()
        item = q.popleft()
        empty_sem.post()
        if item is None:
            print(f"Consumer #{i}: DONE")
            return
        cnt, name = item
        print(f"Consumer #{i}: consuming {cnt} {name}")
        time.sleep(cnt / 10)


def producer(i: int, q: deque, full_sem: MySemaphore, empty_sem: MySemaphore, items: int):
    for _ in range(items):
        cnt = random.randrange(1, 10)
        name = random.choice(ITEMS)
        empty_sem.wait()
        q.append((cnt, name))
        full_sem.post()
        print(f"Producer #{i}: sent {cnt} {name}")
    print(f"Producer #{i}: DONE")


def main(argv):
    try:
        N = int(argv[1])
        P = int(argv[2])
        C = int(argv[3])
        M = int(argv[4])
    except (IndexError, ValueError):
        print(f"usage: {argv[0]} NUM_SLOTS NUM_PRODUCERS NUM_CONSUMERS NUM_ITEMS")
        sys.exit()

    full_sem = MySemaphore(0, max_n=N)
    empty_sem = MySemaphore(N)
    q = deque()

    producers = []
    for i in range(P):
        tp = threading.Thread(target=producer, args=(i + 1, q, full_sem, empty_sem, M))
        tp.start()
        producers.append(tp)

    consumers = []
    for i in range(C):
        tc = threading.Thread(target=consumer, args=(i + 1, q, full_sem, empty_sem))
        tc.start()
        consumers.append(tc)

    for p in producers:
        p.join()

    for _ in consumers:
        empty_sem.wait()
        q.append(None)
        full_sem.post()
    
    for c in consumers:
        c.join()

    full_sem.report("full_sem")
    empty_sem.report("empty_sem")

if __name__ == "__main__":
    main(sys.argv)

