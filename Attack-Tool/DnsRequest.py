from scapy.all import *



 
ip = IP(dst='10.0.2.5', src='10.0.2.7')
udp = UDP(dport=53, sport=53, chksum=0)
dns = DNS(rd=1, qd = DNSQR(qname='thgys.example.com') )


sr1(ip/udp/dns)
