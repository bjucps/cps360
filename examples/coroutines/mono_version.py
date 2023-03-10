#!/usr/bin/env python3
import socket

def main():
    host, port = "localhost", 6060
    sock = socket.create_server((host, port), reuse_port=True)
    print(f"listening on {host}:{port}...")
    
    while True:
        print(f"accepting connection...")
        conn, addr = sock.accept()
        print(f"got connection from {addr}")

        while True:
            print(f"{addr}: reading data...") 
            recd = conn.recv(1024)
            if not recd:
                print(f"{addr}: got EOF; dropping connection...")
                conn.close()
                break

            print(f"{addr}: echoing {len(recd)} bytes...")
            while recd:
                sent = conn.send(recd)
                recd = recd[sent:]


if __name__ == "__main__":
    main()
