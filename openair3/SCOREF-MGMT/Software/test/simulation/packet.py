
import socket
import array
import random

class Packet:
	@staticmethod
	def sendConfigurationRequest(serverAddress, serverPort, clientPort, clientType):
		# Build the packet
		configurationRequestPacket = array.array('B')
		configurationRequestPacket.append(0x40) # Validity=1, version=0
		configurationRequestPacket.append(0x00) # Priority=0
		configurationRequestPacket.append(0x03) # EventType=3
		# Set the event sub-type according to the client type
		if clientType == "GN":
			configurationRequestPacket.append(0x01) # EventSubtype=1
			configurationRequestPacket.append(0xFF) # ConfigurationId=0xFFFF (all)
			configurationRequestPacket.append(0xFF)
		elif clientType == "FAC":
			configurationRequestPacket.append(0x11) # EventSubtype=11
			configurationRequestPacket.append(0xBB) # ConfigurationId=0xBBBB (only FAC)
			configurationRequestPacket.append(0xBB)
		elif clientType == "LTE":
			# This value is not defined yet!
			configurationRequestPacket.append(0x12) # EventSubtype=12
			configurationRequestPacket.append(0xFF) # ConfigurationId=0xFFFF (all)
			configurationRequestPacket.append(0xFF)
		configurationRequestPacket.append(0x00) # TransmissionMode=0x0001 (bulk)
		configurationRequestPacket.append(0x01)

		# Create the socket to send to MGMT
		managementSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
		managementSocket.bind(('0.0.0.0', clientPort))
		sentByteCount = managementSocket.sendto(configurationRequestPacket, (serverAddress, serverPort))
		print sentByteCount, "bytes sent"

		receivedBytes, sourceAddress = managementSocket.recvfrom(1024)
		print receivedBytes.encode('hex'), "bytes received from", sourceAddress

		return True

	@staticmethod
	def sendCommunicationProfileRequest(serverAddress, serverPort, clientPort, clientType):
		# Build the packet
		communicationProfileRequestPacket = array.array('B')
		communicationProfileRequestPacket.append(0x40) # Validity=1, version=0
		communicationProfileRequestPacket.append(0x00) # Priority=0
		communicationProfileRequestPacket.append(0x03) # EventType=3
		# Set the event sub-type according to the client type
		if clientType == "GN":
			communicationProfileRequestPacket.append(0x04) # EventSubtype=4
		elif clientType == "FAC":
			communicationProfileRequestPacket.append(0x15) # EventSubtype=15
		elif clientType == "LTE":
			# This value is not defined yet!
			communicationProfileRequestPacket.append(0x16) # EventSubtype=16
		communicationProfileRequestPacket.append(0xC0) # Transport (1-byte)
		communicationProfileRequestPacket.append(0xC0) # Network (1-byte)
		communicationProfileRequestPacket.append(0x80) # Access (1-byte)
		communicationProfileRequestPacket.append(0xF8) # Channel (1-byte)
		communicationProfileRequestPacket.append(random.randint(1, 100)) # Sequence number (1-byte)
		communicationProfileRequestPacket.append(0x00) # Reserved (3-byte)
		communicationProfileRequestPacket.append(0x00)
		communicationProfileRequestPacket.append(0x00)

		# Create the socket to send to MGMT
		managementSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
		managementSocket.bind(('0.0.0.0', clientPort))
		sentByteCount = managementSocket.sendto(communicationProfileRequestPacket, (serverAddress, serverPort))
		print sentByteCount, "bytes sent"

		receivedBytes, sourceAddress = managementSocket.recvfrom(1024)
		print receivedBytes.encode('hex'), "bytes received from", sourceAddress

		return True

	@staticmethod
	def sendCommunicationProfileSelectionRequest(serverAddress, serverPort, clientPort, clientType):
		# Build the packet
		communicationProfileRequestPacket = array.array('B')
		communicationProfileRequestPacket.append(0x40) # Validity=1, version=0
		communicationProfileRequestPacket.append(0x00) # Priority=0
		communicationProfileRequestPacket.append(0x03) # EventType=3
		communicationProfileRequestPacket.append(0x17) # EventSubtype=17
		communicationProfileRequestPacket.append(0x01) # Latency (1-byte)
		communicationProfileRequestPacket.append(0x05) # Relevance (1-byte)
		communicationProfileRequestPacket.append(0x02) # Reliability (1-byte)
		communicationProfileRequestPacket.append(random.randint(1, 100)) # Sequence number (1-byte)

		# Create the socket to send to MGMT
		managementSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
		managementSocket.bind(('0.0.0.0', clientPort))
		sentByteCount = managementSocket.sendto(communicationProfileRequestPacket, (serverAddress, serverPort))
		print sentByteCount, "bytes sent"

		receivedBytes, sourceAddress = managementSocket.recvfrom(1024)
		print receivedBytes.encode('hex'), "bytes received from", sourceAddress

		return True

	@staticmethod
	def sendNetworkState(serverAddress, serverPort, clientPort):
		# Build the packet
		networkStatePacket = array.array('B')
		networkStatePacket.append(0x40) # Validity=1, version=0
		networkStatePacket.append(0x00) # Priority=0
		networkStatePacket.append(0x04) # EventType=4
		networkStatePacket.append(0x04) # EventSubtype=4
		networkStatePacket.append(0x50) # Timestamp (32-bit)
		networkStatePacket.append(0xA3)
		networkStatePacket.append(0x26)
		networkStatePacket.append(0xF3)
		networkStatePacket.append(0x00) # RxPackets
		networkStatePacket.append(0x00)
		networkStatePacket.append(0x01)
		networkStatePacket.append(0xF0)
		networkStatePacket.append(0x00) # RxBytes
		networkStatePacket.append(0x00)
		networkStatePacket.append(0x02)
		networkStatePacket.append(0x02)
		networkStatePacket.append(0x00) # TxPackets
		networkStatePacket.append(0x00)
		networkStatePacket.append(0x0A)
		networkStatePacket.append(0x56)
		networkStatePacket.append(0x00) # TxBytes
		networkStatePacket.append(0x00)
		networkStatePacket.append(0x10)
		networkStatePacket.append(0x89)
		networkStatePacket.append(0x00) # ToUpperLayerPackets
		networkStatePacket.append(0x00)
		networkStatePacket.append(0x00)
		networkStatePacket.append(0x20)
		networkStatePacket.append(0x00) # DiscardedPackets
		networkStatePacket.append(0x00)
		networkStatePacket.append(0x00)
		networkStatePacket.append(0x0A)
		networkStatePacket.append(0x00) # DuplicatePackets
		networkStatePacket.append(0x00)
		networkStatePacket.append(0x00)
		networkStatePacket.append(0x0C)
		networkStatePacket.append(0x00) # ForwardedPackets
		networkStatePacket.append(0x01)
		networkStatePacket.append(0x30)
		networkStatePacket.append(0x00)

		# Create the socket to send to MGMT
		managementSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
		managementSocket.bind(('0.0.0.0', clientPort))
		sentByteCount = managementSocket.sendto(networkStatePacket, (serverAddress, serverPort))
		print sentByteCount, "bytes sent"

		return True

	@staticmethod
	def sendWirelessState(serverAddress, serverPort, clientPort, clientType):
		random.seed()

		# Build the packet
		wirelessStatePacket = array.array('B')
		wirelessStatePacket.append(0x40) # Validity=1, version=0
		wirelessStatePacket.append(0x00) # Priority=0
		wirelessStatePacket.append(0x04) # EventType=4
		# Set the packet content according to the client type
		if clientType == "GN":
			wirelessStatePacket.append(0x03) # EventSubtype=3
			wirelessStatePacket.append(0x01) # IF Count
			wirelessStatePacket.append(0x00) # Reserved
			wirelessStatePacket.append(0x00) # Reserved
			wirelessStatePacket.append(0x00) # Reserved
			wirelessStatePacket.append(0x00) # Interface ID (2-byte)
			wirelessStatePacket.append(random.randint(1, 10))
			wirelessStatePacket.append(0x00) # Access Technology (2-byte)
			wirelessStatePacket.append(random.randint(1, 255))
			wirelessStatePacket.append(random.randint(1, 255)) # Channel Frequency (2-byte)
			wirelessStatePacket.append(random.randint(1, 255))
			wirelessStatePacket.append(0x00) # Bandwidth (2-byte)
			wirelessStatePacket.append(random.randint(1, 255))
			wirelessStatePacket.append(random.randint(1, 100)) # Channel Busy Ratio (1-byte)
			wirelessStatePacket.append(random.randint(1, 10)) # Status (1-byte)
			wirelessStatePacket.append(random.randint(1, 100)) # Average TX Power (1-byte)
			wirelessStatePacket.append(0x00) # Reserved
		elif clientType == "LTE":
			wirelessStatePacket.append(0x23) # EventSubtype=23
			wirelessStatePacket.append(0x00) # Interface ID (2-byte)
			wirelessStatePacket.append(0x02)
			wirelessStatePacket.append(0x00) # Reserved
			wirelessStatePacket.append(0x00) # Reserved
			wirelessStatePacket.append(0x00) # Reference Signal Received Power (2-byte)
			wirelessStatePacket.append(random.randint(1, 100))
			wirelessStatePacket.append(0x00) # Reference Signal Received Quality (2-byte)
			wirelessStatePacket.append(random.randint(1, 100))
			wirelessStatePacket.append(random.randint(1, 10)) # Channel Quality Indication (1-byte)
			wirelessStatePacket.append(0x01) # Status
			wirelessStatePacket.append(0x00) # Reserved
			wirelessStatePacket.append(0x00) # Reserved
			wirelessStatePacket.append(0x00) # Packet Loss Rate (4-byte)
			wirelessStatePacket.append(0x00)
			wirelessStatePacket.append(random.randint(1, 10))
			wirelessStatePacket.append(random.randint(1, 10))

		# Create the socket to send to MGMT
		managementSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
		managementSocket.bind(('0.0.0.0', clientPort))
		sentByteCount = managementSocket.sendto(wirelessStatePacket, (serverAddress, serverPort))
		print sentByteCount, "bytes sent"

		return True

	@staticmethod
	def sendLocationUpdate(serverAddress, serverPort, clientPort):
		# Build the packet
		locationUpdatePacket = array.array('B')
		locationUpdatePacket.append(0x40) # Validity=1, version=0
		locationUpdatePacket.append(0x00) # Priority=0
		locationUpdatePacket.append(0x01) # EventType=1
		locationUpdatePacket.append(0x10) # EventSubtype=10
		locationUpdatePacket.append(0x01) # Timestamp (4-byte)
		locationUpdatePacket.append(0x02)
		locationUpdatePacket.append(0x03)
		locationUpdatePacket.append(0x04)
		locationUpdatePacket.append(0x01) # Latitude (4-byte)
		locationUpdatePacket.append(0x02)
		locationUpdatePacket.append(0x03)
		locationUpdatePacket.append(0x04)
		locationUpdatePacket.append(0x01) # Longitude (4-byte)
		locationUpdatePacket.append(0x02)
		locationUpdatePacket.append(0x03)
		locationUpdatePacket.append(0x04)
		locationUpdatePacket.append(0x01) # Speed (2-byte)
		locationUpdatePacket.append(0x02)
		locationUpdatePacket.append(0x01) # Heading (2-byte)
		locationUpdatePacket.append(0x02)
		locationUpdatePacket.append(0x01) # Altitude (2-byte)
		locationUpdatePacket.append(0x02)
		locationUpdatePacket.append(0x01) # TAcc, and PodAcc (1-byte)
		locationUpdatePacket.append(0x01) # SAcc, Hacc, and AltAcc (1-byte) 

		# Create the socket to send to MGMT
		managementSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
		managementSocket.bind(('0.0.0.0', clientPort))
		sentByteCount = managementSocket.sendto(locationUpdatePacket, (serverAddress, serverPort))
		print sentByteCount, "bytes sent"

		return True

	@staticmethod
	def sendConfigurationNotification(serverAddress, serverPort, clientPort):
		# Build the packet
		configurationNotificationPacket = array.array('B')
		configurationNotificationPacket.append(0x40) # Validity=1, version=0
		configurationNotificationPacket.append(0x00) # Priority=0
		configurationNotificationPacket.append(0x03) # EventType=3
		configurationNotificationPacket.append(0x14) # EventSubtype=14
		configurationNotificationPacket.append(0x0B) # Configuration ID (2-byte)
		configurationNotificationPacket.append(0xCC)   # Configuration ID = LDM Garbage Collection Interval
		configurationNotificationPacket.append(0x00) # Length (2-byte)
		configurationNotificationPacket.append(0x04)   # Length = 4-byte
		configurationNotificationPacket.append(0x00) # Configuration Value (variable-size)
		configurationNotificationPacket.append(0x00)   # configuration Value = 100 (ms)
		configurationNotificationPacket.append(0x00)
		configurationNotificationPacket.append(0x64)

		# Create the socket to send to MGMT
		managementSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
		managementSocket.bind(('0.0.0.0', clientPort))
		sentByteCount = managementSocket.sendto(configurationNotificationPacket, (serverAddress, serverPort))
		print sentByteCount, "bytes sent"

		return True

	@staticmethod
	def testConfigurationResponse(address):
		# Create the socket to receive from MGMT
		managementSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
#		managementSocket.bind(address)

		receivedBytes = managementSocket.recvfrom(1024)
		print receivedBytes, "bytes received"

		return True
