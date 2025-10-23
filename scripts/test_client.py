import socket

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect(("ohatal.local", 0xAA4A))

s.sendall(b"Hello, server!")