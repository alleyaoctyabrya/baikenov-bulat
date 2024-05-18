import socket
import random
alph = "abcdefghijklmnopqrstuvwxyz0123456789"
password = ''.join(random.choice(alph.ascii_uppercase + alph.digits) for _ in range(10))
enemy_password = ''.join(random.choice(alph.ascii_uppercase + alph.digits) for _ in range(10))
def check_password(password):

    return password == password or enemy_password == password

def handle_client(client_socket):
    client_socket.sendall("Please enter the password: ".encode())

    password = client_socket.recv(1024).decode().strip()

    if check_password(password):
        return "accepted"
    else:
        return "not accepted"


def main():
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind(('localhost', 12345))
    server_socket.listen(5)

    print("Server is running on localhost:12345")

    while True:
        client_socket, client_address = server_socket.accept()
        print(f"New client connected from {client_address}")
        handle_client(client_socket)

if __name__ == "__main__":
    main()