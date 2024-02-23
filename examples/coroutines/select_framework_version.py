#!/usr/bin/env python3
import socket
import select
import signal
import traceback


class Dispatcher:
    def __init__(self):
        self.sockmap = {}

    def register(self, protocol):
        self.sockmap[protocol.fileno()] = protocol

    def unregister(self, protocol):
        del self.sockmap[protocol.fileno()]

    def poll(self, timeout=None):
        rset, wset = [], []
        for fd, protocol in self.sockmap.items():
            if protocol.wants_read():
                rset.append(fd)
            if protocol.wants_write():
                wset.append(fd)

        ract, wact, _ = select.select(rset, wset, [], timeout)
        for rfd in ract:
            try:
                self.sockmap[rfd].on_readable()
            except KeyError:
                pass # this one must have been dropped
            except:
                traceback.print_exc()
                
        for wfd in wact:
            try:
                self.sockmap[wfd].on_writable()
            except KeyError:
                pass # this one must have been dropped
            except:
                traceback.print_exc()


class Protocol:
    def __init__(self, sock, dispatcher):
        self._sock = sock
        self._disp = dispatcher

    @property
    def dispatcher(self):
        return self._disp

    def fileno(self) -> int:
        return self._sock.fileno()

    def close(self):
        self._disp.unregister(self)
        self._sock.close()

    def wants_read(self) -> bool:
        return True

    def wants_write(self) -> bool:
        return False

    def on_readable(self):
        raise NotImplementedError()
    
    def on_writable(self):
        raise NotImplementedError()


class EchoServerProtocol(Protocol):
    def on_readable(self):
        conn, addr = self._sock.accept()
        print(f"Got new client from {addr}")
        self.dispatcher.register(EchoSessionProtocol(conn, self.dispatcher))


class EchoSessionProtocol(Protocol):
    def __init__(self, sock, dispatcher):
        super().__init__(sock, dispatcher)
        self._peer = sock.getpeername()
        self._buff = bytearray()

    def wants_read(self) -> bool:
        return (len(self._buff) < 1024)

    def wants_write(self) -> bool:
        return len(self._buff) > 0

    def on_readable(self):
        print(f"{self._peer}: reading data...")
        recd = self._sock.recv(1024)
        if not recd: # EOF
            print(f"{self._peer}: Got EOF; dropping ...")
            self.close()
            return

        self._buff += recd

    def on_writable(self):
        print(f"{self._peer}: echoing {len(self._buff)} bytes...")
        sent = self._sock.send(self._buff)
        del self._buff[:sent]


def main():
    host, port = "localhost", 6060

    server_sock = socket.create_server((host, port), reuse_port=True)
    server_sock.setblocking(False)

    dispatcher = Dispatcher()
    dispatcher.register(EchoServerProtocol(server_sock, dispatcher))

    done = False
    def _handle_ctrl_c(signum, frame):
        nonlocal done
        done = True
        print(f"Got signal #{signum}; terminating server loop...")
    signal.signal(signal.SIGINT, _handle_ctrl_c)

    print(f"Listening for connections on {host}:{port}...")
    while not done:
        dispatcher.poll(1.0)

if __name__ == "__main__":
    main()
