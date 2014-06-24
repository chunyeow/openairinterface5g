include Makefile.inc
OBJS=$(addprefix MESSAGES/,${ASN_MODULE_SOURCES:.c=.o})

CFLAGS += -DEMIT_ASN_DEBUG=1
CFLAGS += -Wall
CFLAGS += -g


OBJS += x2ap_decoder.o
OBJS += x2ap_encoder.o
OBJS += x2ap_common.o

#X2AP_SERVER_SRC  = sctp_primitives_server.c
#X2AP_SERVER_SRC += x2ap_server_main.c
#X2AP_SERVER_OBJS = $(X2AP_SERVER_SRC:.c=.o)

X2AP_CLIENT_SRC  = sctp_primitives_client.c
X2AP_CLIENT_SRC += x2ap_client_main.c
X2AP_CLIENT_OBJS = $(X2AP_CLIENT_SRC:.c=.o)


libx2ap_messages.a:$(OBJS)
		$(AR) rcsv libx2ap_messages.a $(OBJS)

asn1fromtool:
	 asn1c -gen-PER -fnative-types -fskeletons-copy X2AP-CommonDataTypes.asn X2AP-Constants.asn X2AP-IEs.asn \
	X2AP-PDU.asn


asn1fromscript:
	python ../asn1tostruct.py -f X2AP-PDU-Contents.asn -d

all: server x2ap_client_main

server: server.o libx2ap_messages.a generating_messages.o sctp_primitives_client.o x2ap_pdu_decoder.o
	$(CC) $(CFLAGS) $(LDFLAGS) $(LIBS) -o $@ $^ -lsctp -lpthread libx2ap_messages.a


$(OBJS): %.o : %.c
	$(CC) $(CFLAGS) -I. -o $@ -c $<


$(X2AP_SERVER_OBJS): %.o : %.c
	$(CC) -c $(CFLAGS) -I. $< -o $@

$(X2AP_CLIENT_OBJS): %.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@

x2ap_server_main.o: x2ap_server_main.c 
	$(CC) $(CFLAGS) -I. $^ -o $@ -lsctp \
	-lpthread

x2ap_client_main: $(X2AP_CLIENT_OBJS)
	$(CC) $(CFLAGS) $^ -o $@ -lsctp \
	-lpthread

server.o: server.c
	$(CC) $(CFLAGS) -I. $^ -o $@ -lsctp \
	-lpthread



x2ap_pdu_decoder.o: x2ap_pdu_decoder.c 
	$(CC) $(CFLAGS) -I. -o $@ -c $<

x2ap_pdu_encoder.o: x2ap_pdu_encoder.c
	$(CC) $(CFLAGS) -I. -o $@ -c $<

sctp_primitives_server.o: sctp_primitives_server.c
	$(CC) $(CFLAGS) -o $@ -c $<


generating_messages.o: generating_messages.c
	$(CC) $(CFLAGS) -I. -o $@ -c $<



clean:
	rm -f $(OBJS)


