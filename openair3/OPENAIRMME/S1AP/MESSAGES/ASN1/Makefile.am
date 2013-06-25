BUILT_SOURCES=ieregen.stamp

ieregen.stamp: $(top_srcdir)/S1AP/MESSAGES/ASN1/$(ASN1DIR)/S1AP-PDU-Contents.asn $(top_srcdir)/S1AP/MESSAGES/ASN1/asn1tostruct.py $(top_srcdir)/configure
	echo Timestamp > $@
	python $(top_srcdir)/S1AP/MESSAGES/ASN1/asn1tostruct.py -f$(top_srcdir)/S1AP/MESSAGES/ASN1/$(ASN1DIR)/S1AP-PDU-Contents.asn -o$(top_builddir)/S1AP

clean-local:
	rm -f ieregen.stamp