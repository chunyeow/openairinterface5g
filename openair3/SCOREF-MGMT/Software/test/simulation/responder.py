import threading
import socket

class Responder(threading.Thread):
	def run(self):
		# Create the socket to receive from clients
		managementSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
		while True:
			print "Reading from socket..."
			receivedBytes,senderAddress = managementSocket.recvfrom(1024)
			print receivedByteCount, "bytes received"

