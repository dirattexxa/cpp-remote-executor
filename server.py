import socket
import struct

server_ip = "0.0.0.0" 
server_port = 8080

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.bind((server_ip, server_port))
server.listen(1)

print(f"[Сервер запущен")

client_socket, client_address = server.accept()
print(f" Подключился клиент: {client_address}")

try:
    size_data = client_socket.recv(4)
    if size_data:
        image_size = struct.unpack("i", size_data)[0]
        print(f" Ожидаемый размер картинки {image_size} байт")

        image_bytes = b""
        while len(image_bytes) < image_size:
            chunk = client_socket.recv(image_size - len(image_bytes))
            if not chunk:
                break
            image_bytes += chunk

        with open("received_photo.jpg", "wb") as f:
            f.write(image_bytes)
        print(" Фото успешно получено")

except Exception as e:
    print(f"[Ошибка при приеме данных: {e}")
finally:
    client_socket.close()
    server.close()