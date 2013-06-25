import sys
import signal
import time
from packet import * # Packet generator for MGMT -> CLIENT packets
from responder import * # Responder for CLIENT -> MGMT packets

# Handle SIGINT to exit nicely
def signal_handler(signal, frame):
	print 'CTRL+C caught, exiting...'
	sys.exit(0)
signal.signal(signal.SIGINT, signal_handler)

# Check command-line parameters
if len(sys.argv) == 1:
	print "Usage: ", sys.argv[0], " <scenario-file>"
	sys.exit()
else:
	print "Running scenario file: ", sys.argv[1]

scenarioFile = open(sys.argv[1])

# Server information
serverPort = 1402
serverAddress = "127.0.0.1"
# Client information
clientType = "UNDEFINED"
clientPacketType = ""
clientPort = 8000
# If incoming packets are going to be replied with 
# corresponding response packets or simply ignored
# False: Ignore, True: Process and Reply
clientReply = False

# Traverse scenario file line by line
line = scenarioFile.readline()
line = line.strip('\n')

while line:
	commands = line.split()
	print "DEBUG = ", commands

	# We are asked to SEND a packet
	if commands[0] == "SEND":
		clientPacketType = commands[1]
		print clientPacketType, "packet is going to be sent"

		# Configuration Request Packet
		if clientPacketType == "GET_CONFIGURATION":
			# Ignore the command if the client type is not defined
			if clientType == "UNDEFINED":
				print "ERROR: Client type has to be defined in advance to send a CONFIGURATION_REQUEST"
				print "Ignoring this command..."

			elif Packet.sendConfigurationRequest(serverAddress, serverPort, clientPort, clientType):
				print "CONFIGURATION_REQUEST packet sent successfully"
			else:
				print "ERROR: Cannot send CONFIGURATION_REQUEST"

		# Communication Profile Request Packet
		elif clientPacketType == "COMMUNICATION_PROFILE_REQUEST":
			# Ignore the command if the client type is not defined
			if clientType == "UNDEFINED":
				print "ERROR: Client type has to be defined in advance to send a CONFIGURATION_REQUEST"
				print "Ignoring this command..."

			elif Packet.sendCommunicationProfileRequest(serverAddress, serverPort, clientPort, clientType):
				print "COMMUNICATION_PROFILE_REQUEST packet sent successfully"
			else:
				print "ERROR: Cannot send COMMUNICATION_PROFILE_REQUEST"

		# Communication Profile Selection Request Packet
		elif clientPacketType == "COMMUNICATION_PROFILE_SELECTION_REQUEST":
			# Ignore the command if the client type is not defined
			if clientType != "FAC":
				print "ERROR: Client type has to be defined in advance to send a CONFIGURATION_REQUEST"
				print "Ignoring this command..."

			elif Packet.sendCommunicationProfileSelectionRequest(serverAddress, serverPort, clientPort, clientType):
				print "COMMUNICATION_PROFILE_REQUEST packet sent successfully"
			else:
				print "ERROR: Cannot send COMMUNICATION_PROFILE_REQUEST"

		# Network State Packet
		elif clientPacketType == "NETWORK_STATE":
			if Packet.sendNetworkState(serverAddress, serverPort, clientPort):
				print "NETWORK_STATE packet sent successfully"
			else:
				print "ERROR: Cannot send NETWORK_STATE"

		# Wireless State Packet
		elif clientPacketType == "WIRELESS_STATE":
			if Packet.sendWirelessState(serverAddress, serverPort, clientPort, clientType):
				print "WIRELESS_STATE packet sent successfully"
			else:
				print "ERROR: Cannot send WIRELESS_STATE"

		# Location Update
		elif clientPacketType == "LOCATION_UPDATE":
			if Packet.sendLocationUpdate(serverAddress, serverPort, clientPort):
				print "LOCATION_UPDATE packet sent successfully"
			else:
				print "ERROR: Cannot send LOCATION_UPDATE"

		# Configuration Notification
		elif clientPacketType == "CONFIGURATION_NOTIFICATION":
			if Packet.sendConfigurationNotification(serverAddress, serverPort, clientPort):
				print "CONFIGURATION_NOTIFICATION packet sent successfully"
			else:
				print "ERROR: Cannot send CONFIGURATION_NOTIFICATION"

		# Unknown packet
		else:
			print "Sorry dude, I don't know what you mean by", clientPacketType

	# Wait command
	elif commands[0] == "WAIT":
		howManySeconds = int(commands[1])
		print "Waiting for", howManySeconds, "seconds"
		time.sleep(howManySeconds)

	# DEFINE command family
	elif commands[0] == "DEFINE":		
		# Server address is being defined
		if commands[1] == "SERVER_ADDRESS":
			serverAddress = commands[2]
			print "Server address defined as", serverAddress

		# Server port is being defined
		elif commands[1] == "SERVER_PORT":
			serverPort = int(commands[2])
			print "Server port defined as", serverPort

		# Client port is being defined
		elif commands[1] == "CLIENT_PORT":
			clientPort = int(commands[2])
			print "Client port defined as", clientPort

		# Client type is being defined
		elif commands[1] == "CLIENT_TYPE":
			clientType = commands[2]
			print "Client type defined as", clientType

		# Are incoming packets going to be responded
		elif commands[1] == "REPLY":
			clientReply = True if commands[2] == "TRUE" else False
			if clientReply:
				print "Those incoming messages that require a response will be handled and a response will be sent"
				responder = Responder()
	# TODO			responder.start()
			else:
				print "Those incoming messages that require a response will be ignored"

		# Unknown sub-command
		else:
			print "Sorry dude, I don't know what you mean by", command[1]

	# Unknown command
	else:
		print "Sorry dude, I don't know what you mean by", command[0], "command"

	# Read a new line and remove newline
	line = scenarioFile.readline()
	line = line.strip('\n')

sys.exit()
