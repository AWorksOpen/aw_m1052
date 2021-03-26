from socket import *

#TCP 客户端
s = socket(AF_INET,SOCK_STREAM)
s.connect(("192.168.12.34",5000))  
s.send("micropython")               
s.close()

#TCP 服务器
s = socket(AF_INET,SOCK_STREAM)
s.bind(getaddrinfo("192.168.12.200",8000)[0][-1])
s.listen(2)
sock,addr = s.accept()
sock.recv(5)
s.close()


#其他接口
sock.recvfrom(5)
sock.send("micropython")
sock.sendall("micropython")
sock.sendto("micropython",addr)
sock.write("123")
sock.read(5)
sock.readline()
buf = bytearray(5)
sock.readinto(buf)

#未实现的接口
sock.makefile()

#需要 2.x lwip支持的接口
s.setblocking(True)
s.settimeout(1)
s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)



