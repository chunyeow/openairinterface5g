#!/bin/sh
rm -f $1/logs/pcap2msc.output ;

python $1/extras/output-process/pcap2msc $1/logs/lma2mags.$run_version.pcap \
			$1/logs/lma2cn.$run_version.pcap \
			$1/logs/mag12ap.$run_version.pcap \
			$1/logs/mag12lma.$run_version.pcap  \
			$1/logs/mag22ap.$run_version.pcap \
			$1/logs/mag22lma.$run_version.pcap    > $1/logs/pcap2msc.$run_version.output ;
sync;

python $1/extras/output-process/pcap2msc_check.py  $1/logs/pcap2msc.$run_version.output

python $1/extras/output-process/pcap2msc_filter.py $1/logs/pcap2msc.$run_version.output


cat $1/logs/pcap2msc.$run_version.output.icmp_filtered | mscgen -T png -o $1/logs/pmip_icmp_filtered.$run_version.png


eog $1/logs/pmip_icmp_filtered.$run_version.png &
