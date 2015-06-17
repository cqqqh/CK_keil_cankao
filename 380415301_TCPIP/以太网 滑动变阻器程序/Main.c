//---------------------------------------------------------------------------
// Copyright (c) 2002 Jim Brady
// Do not use commercially without author's permission
// Last revised August 2002
// Net MAIN.C
//
// 8051 Web Server project
// See Makefile for build notes 
// Written for Keil C51 V5.1 compiler, notes:
//   It uses big endian order, which is the same as the
//   network byte order, unlike x86 systems.
//   Use OPTIMIZE(2)or higher so that automatic variables get shared
//   between functions, to stay within the 256 bytes idata space
//---------------------------------------------------------------------------

#include <string.h>
//#include <stdlib.h>
#include "C8051f340.h"
#include "net.h"
//#include "eth.h"
#include "cp220x_reg.h"
#include "timer.h"
#include "analog.h"
#include "arp.h"
#include "tcp.h"
#include "http.h"
#include "ip.h"
MACADDRESS xdata MYMAC; 
sbit AB4_RST = P1^0;                        // Set P1.0 High


UINT idata air_temperature=1;

// Global variables
UINT volatile event_word;
char xdata text[20];  
UCHAR idata debug;
UCHAR idata rcve_buf_allocated;
#define LINK_ERROR  0x20
char xdata inbuf1[1500] _at_ 1000;
char xdata outbuf1[1500] _at_ 2500;

// This sets my hardware address to 00:01:02:03:04:05
UCHAR xdata my_hwaddr[6] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05}; 

// Hardware addr to send a broadcast
UCHAR code broadcast_hwaddr[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// This sets my IP address to 192.168.0.10
ULONG code  my_ipaddr = 0xC0A8837EL;	   //192.168.131.126

// This sets my subnet mask to 255.255.255.0
ULONG code my_subnet = 0xFFFFFF00L;

// Set to 0 if no gateway is present on network
ULONG code gateway_ipaddr = 0xC0A883FEL;  //192.168.131.254

//--------------------------------------------------------------------------
// Initialize the memory management routines
// Initialize variables declared in main
//--------------------------------------------------------------------------

unsigned int Count1msInc;
unsigned char Count1ms,Count10ms,Count1s;
unsigned char TimeSecond,TimeMinute;
unsigned char PHY_Init(void);
void MAC_Init(void);
UCHAR xdata * rcve_frame(void);
void eth_rcve(UCHAR xdata * inbuf);
void MAC_Write(unsigned char mac_reg_offset, unsigned int mac_reg_data);
void MAC_SetAddress(MACADDRESS* pMAC);
void CP220x_Send( UCHAR xdata * outbuf, UINT len);


void int2str(int x, char* str)
{
    int i=1;
    int tmp=10;
    while(x/tmp!=0)
    {
        i++;
        tmp*=10;
    }
    tmp=x;
    str[i]='\0';
    while(i>1)
    {
        str[--i]='0'+(tmp%10);
        tmp/=10;
    }
    str[0]=tmp+'0';
}

void init_main(void)
{
	// Start the memory pool for incoming and outgoing Ethernet
	// frames at 1000, with length = 1500 bytes. Memory below 500
	// is used for program variables
//	init_mempool((void xdata *)1000, 1500);
	memset(text, 0, sizeof(text));
	event_word = 0;
	rcve_buf_allocated = FALSE;
	debug = FALSE;
}

void PORT_Init (void)
{
    XBR0    = 0x01;                     // Enable UART on P0.4(TX) and P0.5(RX)
    P0MDIN    = 0xFC;
    P0SKIP    = 0x03;

   XBR1    = 0x40;                     // Enable Crossbar and weak pull-ups
   P0MDOUT |= 0x10;                    // Enable TX0 as push-pull output
   P1MDOUT |= 0xC0;                    // /WR and /RD are push-pull
   P1MDOUT |= 0x02;					   //P2.5 are push_pull
   P1MDOUT |= 0x18;                    // AB4 LEDs are push-pull
   P1MDOUT &= ~0x01;                   // /RST is open-drain
      
   // Configure External Memory Interface Pins to push-pull except for
   // P2.0 and P2.1 which are used for target board switches.
   P2MDOUT |= 0xFC;                    // ADDR[15:8]

   P1MDIN   = 0xFD;                    // P1.2 为模拟输入
   P1SKIP   = 0x02;                    // P1.2 跳过

   P3MDOUT |= 0xFF;                    // ADDR[7:0]
   P4MDOUT |= 0xFF;                    // DATA[7:0]
   
   // Set initial values of External Memory Interface Pins
/*   P1 = 0xFE;                          // /WR, /RD, are high, RESET is low
   P1 &= ~0x18;                        // LEDs off
   P2 = 0xFF;                          // ADDR[15:8] initially high 
   P3 = 0xFF;                          // ADDR[7:0] initially high
   P4 = 0xFF;  */                        // DATA[7:0] intially high
 EMI0CF = 0x0B;                      // non-muxed mode; split mode 
                                       // with bank select
  EMI0TC = 0xFF;                      // slowest timing (4-cycle MOVX)

//   EMI0CN = 0x20;                      // Off-chip
}



void Timer0_Init (void)
{
   	CKCON|=0x8;
   	TMOD|=0x1;    	//16Bit

	Count10ms=10;
	Count1s=0;
   	TR0 = 0;                         	    // STOP Timer0
   	TH0 = (-SYSCLK/1000/12) >> 8;    		// set Timer0 to overflow in 1ms
   	TL0 = -SYSCLK/1000/12;
   	TR0 = 1;   	// START Timer0
   	IE|= 0x2; 
}
void SYSCLK_Init (void)
{
    unsigned int iTmp;

    //Enable internal high speed oscillator
    OSCICN |= 0x83;
    //Enable internal 4X MULL
    if( !( CLKMUL & ( 1<<5 ) ) )
    {
        CLKMUL = 0;
        CLKMUL = 0x00;
        CLKMUL |= ( 1<<7 );
        for( iTmp = 0; iTmp < 200; iTmp ++ );
        CLKMUL |= ( 1<<6 ) | ( 1<<7 );
        for( iTmp = 0; iTmp < 2000; iTmp ++ );
        while( !( CLKMUL & ( 1<<5 ) ) );
    }
    //USB clock: 48MHz, system clock : 48MHz
    CLKSEL = 0x03;
}


void UART0_Init (void)
{
   SCON0    |= 0x10;                                                            
   CKCON     = 0x70;
   TH1       = 0x30;			//波特率为９６００
   TL1       = TH1;                         
   TMOD      = 0x20;                     
   TR1       = 1;                           
   TI0       = 1; 
   ES0=1;                       
} 





//-----------------------------------------------------

// 函数功能：循环方式发送字符串

// 输出参数：pSendString,char*:欲发送的字符串的缓冲区

// 输出：无

//-----------------------------------------------------

void SendString(char *pSendString)

{

	// 循环发送，首先关闭中断

	ES0 =0;

	while(*pSendString!=0)
	{

		SBUF0 = *pSendString;

		pSendString++;

		while(TI0==0);

		TI0=0;

	}
	ES0=1;

}  



void serial () interrupt 4 

{    

   if(RI0)

  { RI0=0;

   air_temperature=SBUF0;

  }
  else
  TI0=0;

}



void Timer0_ISR (void) interrupt 1  //1ms
{
	TH0 = (-SYSCLK/1000/12) >> 8;  
   	TL0 = -SYSCLK/1000/12;
	if (Count1ms) Count1ms--;
	Count1msInc++;
	if (Count10ms) Count10ms--;
	else
	{
		Count10ms=10;    			//10ms
		if (Count1s) Count1s--;
		else
		{
			Count1s=100;			//1s
			TimeSecond++;
			if (TimeSecond>=60)
			{
				TimeSecond=0;		//1min
				TimeMinute++;
				if	(TimeMinute==60)	TimeMinute=0;
			}
		}
	}
}


void Delay1ms(unsigned char T)
{
	Count1ms=T;
	while (Count1ms);
}

  

void LightONOFF(bit b)
{
  b=1;
}

void CP220x_RST_Low(void);
	//加延时
 void   CP220x_RST_High(void);




//////////////////////////////////////////////////////////////////////
void main (void)
{
	UINT  event_word_copy;
	UCHAR xdata * inbuf;
	unsigned char error_code;
//	unsigned int num_bytes;
          	
    PCA0MD &= ~0x40;                    // WDTE = 0 (clear watchdog timer
                                       // enable)


	SYSCLK_Init ();                     // initialize oscillator
	Timer0_Init();
	PORT_Init ();                       // initialize crossbar and GPIO
    UART0_Init();
   
	init_timer2();
	init_main();
	init_tcp();
	init_http();
 	EA=1;
	init_adc();
	init_arp();
	//init_8019();
	CP220x_RST_Low();
	//加延时
	
   Delay1ms(200);
    CP220x_RST_High();
	
  
   Delay1ms(200);

    INT0EN = 0x03;                         
   INT1EN = 0x00;
   
   // Clear all Interrupt Flags by reading the self-clearing status registers                          
//   temp_char = INT0;                      
 //  temp_char = INT1;  
   error_code = PHY_Init();  
   Delay1ms(50);
   Delay1ms(50);
   MAC_Init();    
   Delay1ms(50);
   Delay1ms(50);
  
  	ET2 = 1;		            // Enable timer 2 interrupt
	  RXCN = RXCLEAR;
      EA  =  1;

	  Delay1ms(50);
	  Delay1ms(50);
	  Delay1ms(50);
	  Delay1ms(50);
   Delay1ms(50);
    IP        = 0x01;

  SendString("start");  	
   // The code below is a priority based RTOS.  The event
   // handlers are in priority order - highest priority first.
	while (1)
   {
      // Query CS8900A to see if Ethernet frame has arrived
      // If so, set EVENT_ETH_ARRIVED bit in event_word
     // query_8019();
	/* 12.2. Reading a Packet Using the Autoread Interface
Once reset initialization is complete (Section 6.2 on page 18) and the receive buffer, filter, and hash table (Section
12.4) are initialized, the CP2200/1 is ready to receive Ethernet packets. After receiving notification of a new packet,
the following procedure can be used to read the packet:
Step 1: Read RXVALID (CPINFOH.7) and RXOK (CPINFOL.7) to check if the current packet was received
correctly. The host processor may optionally use the packet starting address CPADDR to read
specific bytes in the packet and determine whether to copy or skip the current packet. The random
access method described in Section 7.1 on page 23 can be used to access the buffer.
Step 2: If RXVALID or RXOK is 0, or to skip the packet, write a ‘1’ to RXSKIP (RXCN.1).
If RXVALID and RXOK are 1, read the length of the current packet from CPLENH:CPLENL.
Step 3: Read the entire packet, one byte at a time, by reading RXAUTORD.
Step 4: If the entire packet was read, write a ‘1’ to RXCLRV (RXCN.2).
If there are any unread bytes remaining in the current buffer, write a ‘1’ to RXSKIP (RXCN.1).*/
	  if(CPINFOH & RXVALID)  //CPINFOH的第7位 
      event_word |= EVENT_ETH_ARRIVED;
      // Use a copy of event word to avoid interference
      // with interrupts
		event_word_copy = event_word;
		EA = 1;
      
      // See if an Ethernet frame has arrived      
      if (event_word_copy & EVENT_ETH_ARRIVED)
      {
         EA = 0;
         event_word &= (~EVENT_ETH_ARRIVED);
         EA = 1;
         
         // Allocate a buffer and read frame from CS8900A
	//	 inbuf = (UCHAR xdata *)malloc(1000);
	//	 num_bytes = CP220x_Receive(inbuf, sizeof(inbuf));
         inbuf = rcve_frame();
           if (inbuf != NULL)
         {

            // Process the received Ethernet frame 
            eth_rcve(inbuf); 
         
            // If the memory allocated for the rcve message has
            // not already been freed then free it now
            if (rcve_buf_allocated)
            {
//               free(inbuf);
               rcve_buf_allocated = FALSE;
            }
         }
          
         
          
       
      }
      
      // See if TCP retransmit timer has expired            	       
      else if (event_word_copy & EVENT_TCP_RETRANSMIT)
      {
         event_word &= (~EVENT_TCP_RETRANSMIT);
         EA = 1;
         tcp_retransmit();
 		}

      // See if TCP inactivity timer has expired
      else if (event_word_copy & EVENT_TCP_INACTIVITY)
      {
         event_word &= (~EVENT_TCP_INACTIVITY);
         EA = 1;
         tcp_inactivity();
 		}

      // See if ARP retransmit timer has expired
 		else if (event_word_copy & EVENT_ARP_RETRANSMIT)
      {
         event_word &= (~EVENT_ARP_RETRANSMIT);
         EA = 1;
         arp_retransmit();
		}

      // See if it is time to age the ARP cache
      else if (event_word_copy & EVENT_AGE_ARP_CACHE)
      {
         event_word &= (~EVENT_AGE_ARP_CACHE);
			EA = 1;
         age_arp_cache();
		}

		// See if it is time to read the analog inputs
		else if (event_word_copy & EVENT_READ_ANALOG)
      {
         event_word &= (~EVENT_READ_ANALOG);
         EA = 1;
			// Read one of the 3 analog inputs each time
			read_analog_inputs();
      }

		// See if an RS232 message has arrived.  It is
      // not handled - RS232 is used for sending only
		else if (event_word_copy & EVENT_RS232_ARRIVED)
      {
         event_word &= (~EVENT_RS232_ARRIVED);
         EA = 1;
      }
   }
}

void CP220x_RST_Low(void)
{
 
}

void CP220x_RST_High(void)
{
 
}

unsigned char PHY_Init()
{
   unsigned char temp_char;
   unsigned char retval = 0;
   
   //--------------------------------------------------------------------------
   // Auto-Negotiation Synchronization (Section 15.2 of CP220x Datasheet)
   //--------------------------------------------------------------------------

   // Step 1: Disable the PHY
   PHYCN = 0x00;  
   
   // Step 2: Enable the PHY with link integrity test and auto-negotiation 
   // turned off
      
      // A. Disable the Transmitter Power Save Option and Configure Options
      TXPWR = 0x80;
      PHYCF = ( SMSQ | JABBER | ADPAUSE | AUTOPOL );

      // B. Enable the Physical Layer
      PHYCN = PHYEN;
   Delay1ms(200);
   Delay1ms(200);
   Delay1ms(200);
      // C. Wait for the physical layer to power up
     // wait_ms(10);

      // D. Enable the Transmitter and Receiver
      PHYCN = ( PHYEN | TXEN | RXEN );
   
   // Step 3: Poll the Wake-on-Lan Interrupt
      Delay1ms(200);
  
      // A. Clear Interrupt Flags
      temp_char = INT1;

      // B. Start a new timeout for 1.5 seconds
//      reset_timeout(ONE_SECOND+ONE_SECOND/2);
      
      // C. Check for a signal
     
         // If no signal is deteced, wait 1.5s, then continue
//         if(timeout_expired()){
//            break;
 //        }

     

   //--------------------------------------------------------------------------
   // Physical Layer Initialization (Section 15.7 of CP220x Datasheet)
   //--------------------------------------------------------------------------
   
   // Step 1: Synchronization procedure implemented above

   // Step 2: Disable the physical layer
   PHYCN = 0x00; 
  
   // Step 3: Configure the desired physical layer options including 
   // auto-negotiation and link integrity
   PHYCF = ( SMSQ | LINKINTG | JABBER | AUTONEG | ADPAUSE | AUTOPOL );
  
   // Step 4: Enable the physcial layer
      
      // A. Enable the Physical Layer
      PHYCN = PHYEN;
 Delay1ms(200);
   Delay1ms(200);
   Delay1ms(200);
      // B. Wait for the physical layer to power up
//      wait_ms(10);

      // C. Enable the Transmitter and Receiver
      // Auto-negotiation begins now
      PHYCN = ( PHYEN | TXEN | RXEN );
  

   // Step 5: Wait for auto-negotiation to complete

      // Clear INT1 Interrupt Flags
      temp_char = INT1;
        Delay1ms(200);
   Delay1ms(200);
   
      // Start a six second timeout
//      reset_timeout(6*ONE_SECOND);
  
      // Check for autonegotiation fail or complete flag
      while(1){
         // If Auto-Negotiation Completes/Fails, break
         if(INT1RD & (ANCINT | ANFINT)){
            break;            
         }
      
       
      }


      // Mask out all bits except for auto negotiation bits 
      temp_char = INT1RD;
      temp_char &= (ANCINT | ANFINT);

      // Check if Auto-Negotiation has FAILED 
      if(temp_char & ANFINT){
      
         // Auto-Negotiation has failed
         retval = LINK_ERROR;

        
   
      } else 
   
      // Check if Auto-Negotiation has PASSED   
      if(temp_char == ANCINT){
      
         // Auto-Negotiation has passed
         retval = 0;

         // Enable Link LED and Activity LED                         
         IOPWR = 0x0C;   
         
                           

      } else 
   
      // Timeout Occured.   
      { 
          // Timeout
         retval = LINK_ERROR;
    
        
      }

   return retval;
     
}

void MAC_Init(void)
{  

   // Check the duplex mode and perform duplex-mode specific initializations
   if(PHYCN & 0x10){
      
      // The device is in full-duplex mode, configure MAC registers
      // Padding is turned on.
      MAC_Write(MACCF, 0x40B3);
      MAC_Write(IPGT, 0x0015);
      
   } else {

      // The device is in half-duplex mode, configure MAC registers
      // Padding is turned off.
      MAC_Write(MACCF, 0x4012);
      MAC_Write(IPGT, 0x0012);

   }

   // Configure the IPGR register
   MAC_Write(IPGR, 0x0C12);

   // Configure the MAXLEN register to 1518 bytes
   MAC_Write(MAXLEN, 0x05EE);

   // Copy MAC Address Stored in Flash to MYMAC
   FLASHADDRH = 0x1F;
   FLASHADDRL = 0xFA;

   MYMAC.Char[0] = FLASHAUTORD;
   MYMAC.Char[1] = FLASHAUTORD;
   MYMAC.Char[2] = FLASHAUTORD;
   MYMAC.Char[3] = FLASHAUTORD;
   MYMAC.Char[4] = FLASHAUTORD;
   MYMAC.Char[5] = FLASHAUTORD;      
   my_hwaddr[0]=MYMAC.Char[0];
   my_hwaddr[1]=MYMAC.Char[1];
   my_hwaddr[2]=MYMAC.Char[2];
   my_hwaddr[3]=MYMAC.Char[3];
   my_hwaddr[4]=MYMAC.Char[4];
   my_hwaddr[5]=MYMAC.Char[5];
   // Program the MAC address
   MAC_SetAddress(&MYMAC); 

   // Enable Reception and configure Loopback mode
   MAC_Write(MACCN, 0x0001);           // Enable Reception without loopback

}

void MAC_Write(unsigned char mac_reg_offset, unsigned int mac_reg_data)
{

   // Step 1: Write the address of the indirect register to MACADDR.
   MACADDR = mac_reg_offset;              

   // Step 2: Copy the contents of <mac_reg_data> to MACDATAH:MACDATAL
   MACDATAH = (mac_reg_data >> 8);    // Copy High Byte
   MACDATAL = (mac_reg_data & 0xFF);  // Copy Low Byte

   // Step 3: Perform a write on MACRW to transfer the contents of MACDATAH:MACDATAL
   // to the indirect MAC register.
   MACRW = 0;
   
   return;
}


//-----------------------------------------------------------------------------
// MAC_SetAddress
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : 
//   1)  MACADDRESS* pMAC - pointer to a 6-byte MAC address structure.
// 
// Sets the current MAC address to the MAC address pointed to by <pMAC>.
//-----------------------------------------------------------------------------
void MAC_SetAddress(MACADDRESS* pMAC)
{
   UINT1 temp_int;

   temp_int.Char[0] = pMAC->Char[5];
   temp_int.Char[1] = pMAC->Char[4];
   MAC_Write(MACAD0, temp_int.Int);
   
   temp_int.Char[0] = pMAC->Char[3];
   temp_int.Char[1] = pMAC->Char[2];
   MAC_Write(MACAD1, temp_int.Int);
   
   temp_int.Char[0] = pMAC->Char[1];
   temp_int.Char[1] = pMAC->Char[0];
   MAC_Write(MACAD2, temp_int.Int);
   
   return;
}



void eth_rcve(UCHAR xdata * inbuf)
{
   ETH_HEADER xdata * eth;
   
   eth = (ETH_HEADER xdata *)inbuf;
   
   // Reject frames in IEEE 802 format where Eth type field
   // is used for length.  Todo: Make it handle this format
   if (eth->frame_type < 1520)
   {
//      free(inbuf);
      return;      
   }

   // Figure out what type of frame it is from Eth header
   // Call appropriate handler and supply address of buffer
   switch (eth->frame_type)
   {
	   case ARP_PACKET:
	   arp_rcve(inbuf);
	   break;
		      
	   case IP_PACKET:
	   ip_rcve(inbuf);
      break;

      default:
	        ;
      break;
   }
}


void eth_send(UCHAR xdata * outbuf, UCHAR * hwaddr, UINT ptype, UINT len)
{
	ETH_HEADER xdata * eth;
   
   eth = (ETH_HEADER xdata *)outbuf;
	  
	// Add 14 byte Ethernet header
	memcpy(eth->dest_hwaddr, hwaddr, 6);
	memcpy(eth->source_hwaddr, my_hwaddr, 6); 
   eth->frame_type = ptype;

   // We just added 14 bytes to length
   CP220x_Send(outbuf, len + 14);
}

void CP220x_Send( UCHAR xdata * outbuf, UINT len)
{

   int i; 
   unsigned int ramaddr;

   // Define Macro to increment the RAM address Pointer
   #define INC_RAMADDR  ramaddr++; \
                        RAMADDRH = (ramaddr >> 8);\
                        RAMADDRL = (ramaddr & 0x00FF);


   // Step 1: Poll TXBUSY until it becomes 0x00
   while(TXBUSY);

   // Step 2: Set the TXSTARTH:TXSTARTL address to 0x0000
   TXSTARTH = 0x00;
   TXSTARTL = 0x00;


   // Step 3: Load data into transmit buffer
   // When the random access method is used, we do not need to check for
   // aborted packets. This method will be slightly slower than the Autowrite
   // method, however, it reduces code space requirements.
  
      // Setup RAM Address Pointer To 0x0000	
      RAMADDRH = 0x00;
      RAMADDRL = 0x00;
      ramaddr = 0x0000;

      // Step 3d: Load the packet payload
      for(i = 0; i < len; i++){
         RAMTXDATA = outbuf[i];
         INC_RAMADDR
      }
      
      // Step 3e: Pad short packets
      while(ramaddr < 64){
         RAMTXDATA = 0;
         INC_RAMADDR
      }
      
      // Set the TXENDH:TXENDL address to <ramaddr - 1>
      ramaddr--;
      TXENDH = (ramaddr >> 8);
      TXENDL = (ramaddr & 0x00FF);


   // Step 4: Set the TXSTARTH:TXSTARTL address back to 0x0000
   TXSTARTH = 0x00;
   TXSTARTL = 0x00;
   
   // Step 5: Write '1' to TXGO to begin transmission
   TXCN = 0x01;
//   free(outbuf);
 
}

UCHAR xdata * rcve_frame(void)

{
   bit rx_ok;
   bit skip = 0;
   UINT1 cplen;   
   unsigned int i;
   UCHAR xdata * buf;

   unsigned char interrupt_read;
   unsigned char valid_bits;
   unsigned char num_packets;

   // Clear interrupt flags.
   interrupt_read = INT1;
   interrupt_read = INT0;
   
   // Check for packet received interrupt
   if( interrupt_read & RXINT)
   {   
      // Count the number of packets in the receive buffer         
      // This is equal to the number of bits set to 1 in TLBVALID
      valid_bits = TLBVALID;     
	    for(num_packets = 0; valid_bits; num_packets++)      
      {                                 
         valid_bits &= valid_bits - 1; 
      }
   
      // If the receive buffer has 7 packets, then disable reception.
      if( num_packets >= 7) 
      {
         RXCN = RXINH;           // Inhibit New Packet Reception
      }
	  }

   // Step 1: Check the RXOK bit to see if packet was received correctly
   rx_ok = (CPINFOL & RXOK) && (CPINFOH & RXVALID);

   // Step 2: If packet received correctly, read the length, otherwise, skip packet.
   if(rx_ok){
   
      // Read the packet length
      cplen.Char[0] = CPLENH;
      cplen.Char[1] = CPLENL;
	  buf=inbuf1;
//	  buf = (UCHAR xdata *)malloc(cplen.Int);
        
   } else {
     
      // Set packet length to zero
      cplen.Int = 0;
      
      // Skip packet
      skip = 1;      
	  buf = NULL;
   }   

   // Step 3: Read the entire packet from the buffer 
   
   // If packet will fit in the buffer
   if(1){
      
      // Copy entire packet
      for(i = 0; i < cplen.Int; i++){
         buf[i] = RXAUTORD;
      }
	  		rcve_buf_allocated = TRUE;


   } else {

      // Set packet length to zero
      cplen.Int = 0;
      
      // Skip packet
      skip = 1; 
   }
      
   // Step 4: Skip the packet, or clear the valid bit if the entire packet
   // has been unloaded from the buffer.
   
   if(skip)
   {
      RXCN |= 0x02;                    // Skip the packet
   } 

   else 
   {
      RXCN |= 0x04;                    // Clear the valid bit only
   }

   // If there are no more packets in the receive buffer, enable reception
   if(TLBVALID == 0x00)
   {
      RXCN = 0x00;   
   }
   
   // Return the number of bytes added to the buffer
   return(buf);
}


