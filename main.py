from socket import socket, AF_INET, SOCK_DGRAM

HOST = ''   
PORT = 56301

# ソケットを用意
s = socket(AF_INET, SOCK_DGRAM)
# バインドしておく
s.bind((HOST, PORT))

while True:
    # 受信
    msg, address = s.recvfrom(1380)
    print(f"message: {msg}\nfrom: {address}")

# ソケットを閉じておく
s.close()