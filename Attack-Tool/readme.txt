This is the readme to run the attack

1) run genDnsReqRes.py to create payloads: ip_req.bin, ip_resp.bin
	- sudo python genDnsReqRes.py

2) compile attack.c
	-sudo gcc attack.c -o attackIPchange

3) run executable 
	- sudo ./attackIPchange

4) watch the dns havoc ensue
