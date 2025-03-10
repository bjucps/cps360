#!/usr/bin/env python3
import select
import socket

def main():
    host, port = "localhost", 6060
    listen_sock = socket.create_server((host, port), reuse_port=True)
    print(f"listening on {host}:{port}...")

    # lists of readable/writable sockets (including listen_sock)
    readables = [listen_sock]
    writables = []

    # only for connection/data sockets (maps ID-of-sock-object to (addr, echoBuff))
    conns = { }
    
    while True:
        # see which sockets can actually read/write right now
        canread, canwrite, _ = select.select(readables, writables, [])

        # handle readable sockets
        for readsock in canread:
            if readsock is listen_sock: 
                # special case: listen_sock readable means we have a connection ready
                new_sock, addr = listen_sock.accept()
                print(f"got connection from {addr}")
                readables.append(new_sock)                  # echo conns are always readable
                conns[id(new_sock)] = (addr, bytearray())   # empty send buffer
            else:
                # normal case: one of our connections has data to read
                addr, buff = conns[id(readsock)]
                print(f"{addr}: reading data...")
                recd = readsock.recv(1024)
                if not recd:
                    # EOF: no data read, that means drop the connection
                    print(f"{addr}: got EOF; dropping connection...")
                    readsock.close()                    # close socket itself
                    readables.remove(readsock)          # remove from readable list
                    if readsock in writables:           # remove from writable list (if on that list)
                        writables.remove(readsock)
                    del conns[id(readsock)]             # remove connection record/metadata
                else:
                    # data: queue it for echoing
                    print(f"{addr}: echoing {len(recd)} bytes...")
                    buff += recd                        # extend buffer object (in-place) with new data
                    if readsock not in writables:       # make this socket writable (if not already)
                        writables.append(readsock)

        # handle writable sockets
        for writesock in canwrite:
            _, buff = conns[id(writesock)]              # look up send-buffer for this connection
            sent = writesock.send(buff)                 # write all the data if we can (track what we actually write)
            del buff[:sent]                             # chop off the transmitted data (in-place)
            if not buff:                                # if the buffer is EMPTY, remove this socket from the writable set (nothing to write)
                writables.remove(writesock)


if __name__ == "__main__":
    main()
