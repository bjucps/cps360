#!/usr/bin/env python3
import selectors
import signal
import socket
import traceback
from collections import deque
from types import coroutine  # plain generators can't be await'd; "coroutines" can


@coroutine
def async_spawn(coro):
    return (yield ('async_spawn', coro))


@coroutine
def async_accept(sock):
    return (yield ('async_accept', sock))


@coroutine
def async_recv(sock, size):
    return (yield ('async_recv', sock, size))


@coroutine
def async_send(sock, buff):
    return (yield ('async_send', sock, buff))


class EventLoop:
    """coroutine/async-I/O event loop dispatcher"""
    def __init__(self):
        self._tasks = deque()
        self._sel = selectors.DefaultSelector()

    def cycle(self, timeout=None):
        """run one cycle of the event loop"""
        # pump I/O events to see if any I/O needs to happen
        for event, _ in self._sel.select(timeout):
            event.data()

        # dispatch all queued coroutines
        while self._tasks:
            coroutine, sendee, throwee = self._tasks.popleft()
            try:
                if throwee is not None:
                    yielded = coroutine.throw(throwee)
                else:
                    yielded = coroutine.send(sendee)
            except StopIteration: # let it die
                continue
            except: # also let it die
                traceback.print_exc()
            else: # invoke one of our async operation methods to queue I/O (etc.)
                # (hat tip to David Beazley's coroutine talks for this idea)
                op, *args = yielded
                getattr(self, op)(coroutine, *args)

    def enqueue(self, coro, sendee=None, throwee=None):
        """public API to add a coroutine into the scheduler queue"""
        self._tasks.append((coro, sendee, throwee))

    def async_spawn(self, parent_coro, new_coro):
        """async op to keep a parent coroutine running _and_ start scheduling a child coroutine"""
        self.enqueue(new_coro)
        self.enqueue(parent_coro)

    def async_accept(self, coro, listener_sock):
        """queue a readable I/O status in the selector so we can "accept"

        sets up a callback to re-schedule the originating coroutine
        with the results of the "accept" once it's done
        """
        def _callback():
            self._sel.unregister(listener_sock)
            try:
                conn, addr = listener_sock.accept()
                conn.setblocking(False)
                self.enqueue(coro, (conn, addr))
            except Exception as ex:
                self.enqueue(coro, None, ex)
        self._sel.register(listener_sock, selectors.EVENT_READ, _callback)

    def async_recv(self, coro, sock, size):
        """queue a readable I/O status in the selector so we can "recv"

        sets up a callback to re-schedule the originating coroutine
        with the results of the "recv" once it's done
        """
        def _callback():
            self._sel.unregister(sock)
            try:
                self.enqueue(coro, sock.recv(size))
            except Exception as ex:
                self.enqueue(coro, None, ex)
        self._sel.register(sock, selectors.EVENT_READ, _callback)

    def async_send(self, coro, sock, buff):
        """queue a writable I/O status in the selector so we can "send"

        sets up a callback to re-schedule the originating coroutine
        with the results of the "send" once it's done
        """
        def _callback():
            self._sel.unregister(sock)
            try:
                self.enqueue(coro, sock.send(buff))
            except Exception as ex:
                self.enqueue(coro, None, ex)
        self._sel.register(sock, selectors.EVENT_WRITE, _callback)

    
async def echo_server(host, port):
    """echo-server as a coroutine for use with EventLoop"""
    sock = socket.create_server((host, port), reuse_port=True)
    sock.setblocking(False)

    print(f"Listening on {host}:{port}")
    
    while True:
        print(f"Accepting connections...")
        conn, addr = await async_accept(sock)
        print(f"Got connection from {addr}; spawning session!")
        await async_spawn(echo_session(conn))


async def echo_session(conn):
    """echo-session as a coroutine for use with EventLoop"""
    peer = conn.getpeername()

    while True:
        print(f"{peer}: reading data...")
        recd = await async_recv(conn, 1024)
        if not recd:
            print(f"EOF from {peer}; dropping...")
            return
       
        print(f"{peer}: echoing {len(recd)} bytes...") 
        while recd:
            sent = await async_send(conn, recd)
            recd = recd[sent:]


def main():
    host, port = "localhost", 6060
    loop = EventLoop()
    loop.enqueue(echo_server(host, port))

    done = False
    def _handle_ctrl_c(signum, frame):
        nonlocal done
        done = True
        print(f"Got signal #{signum}; terminating server loop...")
    signal.signal(signal.SIGINT, _handle_ctrl_c)

    while not done:
        loop.cycle(timeout=1.0)


if __name__ == "__main__":
    main()
