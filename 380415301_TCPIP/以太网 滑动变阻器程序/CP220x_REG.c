//-----------------------------------------------------------------------------
// CP220x_REG.c
//-----------------------------------------------------------------------------
// Copyright 2006 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// CP220x Register Definitions.
//
// FID:
// Target:         Multiple
// Tool chain:     Keil C51 7.20 / Keil EVAL C51
//                 Silicon Laboratories IDE version 2.72
// Command Line:   See Readme.txt
// Project Name:   CP220x_Ethernet_Routines
//
//
//
// Release 1.0
//    -Initial Release (FB)
//    -30 MAY 2006
//
//-----------------------------------------------------------------------------
// Direct Register Definitions
//-----------------------------------------------------------------------------
volatile unsigned char xdata     RAMADDRH     _at_   0x3F08;   // RAM Address Pointer High Byte
volatile unsigned char xdata     RAMADDRL     _at_   0x3F09;   // RAM Address Pointer Low Byte
volatile unsigned char xdata     RAMRXDATA    _at_   0x3F02;   // RXFIFO RAM Data Register
volatile unsigned char xdata     RAMTXDATA    _at_   0x3F04;   // TXBUFF RAM Data Register
volatile unsigned char xdata     FLASHADDRH   _at_   0x3F69;   // Flash Address Pointer High Byte
volatile unsigned char xdata     FLASHADDRL   _at_   0x3F68;   // Flash Address Pointer Low Byte
volatile unsigned char xdata     FLASHAUTORD  _at_   0x3F05;   // Flash Autoread w/ increment
volatile unsigned char xdata     FLASHDATA    _at_   0x3F06;   // Flash Read/Write Data Register
volatile unsigned char xdata     FLASHKEY     _at_   0x3F67;   // Flash Lock and Key
volatile unsigned char xdata     FLASHERASE   _at_   0x3F6A;   // Flash Erase
volatile unsigned char xdata     FLASHSTA     _at_   0x3F7B;   // Flash Status
volatile unsigned char xdata     MACADDR      _at_   0x3F0A;   // MAC Address Pointer
volatile unsigned char xdata     MACDATAH     _at_   0x3F0B;   // MAC Data Register High Byte
volatile unsigned char xdata     MACDATAL     _at_   0x3F0C;   // MAC Data Register Low Byte
volatile unsigned char xdata     MACRW        _at_   0x3F0D;   // MAC Read/Write Initiate
volatile unsigned char xdata     INT0         _at_   0x3F63;   // Interrupt Status Register 0 (Self-Clearing)
volatile unsigned char xdata     INT0RD       _at_   0x3F76;   // Interrupt Status Register 0 (Read-Only)
volatile unsigned char xdata     INT0EN       _at_   0x3F64;   // Interrupt Enable Register 0
volatile unsigned char xdata     INT1         _at_   0x3F7F;   // Interrupt Status Register 1 (Self-Clearing)
volatile unsigned char xdata     INT1RD       _at_   0x3F7E;   // Interrupt Status Register 1 (Read-Only)
volatile unsigned char xdata     INT1EN       _at_   0x3F7D;   // Interrupt Enable Register 1
volatile unsigned char xdata     VDMCN        _at_   0x3F13;   // VDD Monitor Control Register
volatile unsigned char xdata     SWRST        _at_   0x3F75;   // Software Reset Register
volatile unsigned char xdata     RSTSTA       _at_   0x3F73;   // Reset Source Status Register
volatile unsigned char xdata     RSTEN        _at_   0x3F72;   // Reset Enable Register
volatile unsigned char xdata     IOPWR        _at_   0x3F70;   // Port Input/Output Power
volatile unsigned char xdata     OSCPWR       _at_   0x3F7C;   // Oscillator Power
volatile unsigned char xdata     RXFILT       _at_   0x3F10;   // Receive Filter Configuraton
volatile unsigned char xdata     RXCN         _at_   0x3F11;   // Receive Control
volatile unsigned char xdata     RXSTA        _at_   0x3F12;   // Receive Status
volatile unsigned char xdata     RXAUTORD     _at_   0x3F01;   // Receive Autoread w/ increment
volatile unsigned char xdata     RXHASHH      _at_   0x3F0E;   // Receive Hash Table High Byte
volatile unsigned char xdata     RXHASHL      _at_   0x3F0F;   // Receive Hash Table Low Byte
volatile unsigned char xdata     CPINFOH      _at_   0x3F1D;   // Current RX Packet Information High Byte
volatile unsigned char xdata     CPINFOL      _at_   0x3F1E;   // Current RX Packet Information Low Byte
volatile unsigned char xdata     CPLENH       _at_   0x3F1F;   // Current RX Packet Length High Byte
volatile unsigned char xdata     CPLENL       _at_   0x3F20;   // Current RX Packet Length Low Byte
volatile unsigned char xdata     CPADDRH      _at_   0x3F21;   // Current RX Packet Address High Byte
volatile unsigned char xdata     CPADDRL      _at_   0x3F22;   // Current RX Packet Address Low Byte
volatile unsigned char xdata     CPTLB        _at_   0x3F1A;   // Current RX Packet TLB Number
volatile unsigned char xdata     TLBVALID     _at_   0x3F1C;   // TLB Valid Indicators
volatile unsigned char xdata     TLB0INFOH    _at_   0x3F23;   // TLB0 Information High Byte
volatile unsigned char xdata     TLB0INFOL    _at_   0x3F24;   // TLB0 Information Low Byte
volatile unsigned char xdata     TLB0LENH     _at_   0x3F25;   // TLB0 Length High Byte
volatile unsigned char xdata     TLB0LENL     _at_   0x3F26;   // TLB0 Length Low Byte
volatile unsigned char xdata     TLB0ADDRH    _at_   0x3F27;   // TLB0 Address High Byte
volatile unsigned char xdata     TLB0ADDRL    _at_   0x3F28;   // TLB0 Address Low Byte
volatile unsigned char xdata     TLB1INFOH    _at_   0x3F29;   // TLB1 Information High Byte
volatile unsigned char xdata     TLB1INFOL    _at_   0x3F2A;   // TLB1 Information Low Byte
volatile unsigned char xdata     TLB1LENH     _at_   0x3F2b;   // TLB1 Length High Byte
volatile unsigned char xdata     TLB1LENL     _at_   0x3F2C;   // TLB1 Length Low Byte
volatile unsigned char xdata     TLB1ADDRH    _at_   0x3F2D;   // TLB1 Address High Byte
volatile unsigned char xdata     TLB1ADDRL    _at_   0x3F2E;   // TLB1 Address Low Byte
volatile unsigned char xdata     TLB2INFOH    _at_   0x3F2F;   // TLB2 Information High Byte
volatile unsigned char xdata     TLB2INFOL    _at_   0x3F30;   // TLB2 Information Low Byte
volatile unsigned char xdata     TLB2LENH     _at_   0x3F31;   // TLB2 Length High Byte
volatile unsigned char xdata     TLB2LENL     _at_   0x3F32;   // TLB2 Length Low Byte
volatile unsigned char xdata     TLB2ADDRH    _at_   0x3F33;   // TLB2 Address High Byte
volatile unsigned char xdata     TLB2ADDRL    _at_   0x3F34;   // TLB2 Address Low Byte
volatile unsigned char xdata     TLB3INFOH    _at_   0x3F35;   // TLB3 Information High Byte
volatile unsigned char xdata     TLB3INFOL    _at_   0x3F36;   // TLB3 Information Low Byte
volatile unsigned char xdata     TLB3LENH     _at_   0x3F37;   // TLB3 Length High Byte
volatile unsigned char xdata     TLB3LENL     _at_   0x3F38;   // TLB3 Length Low Byte
volatile unsigned char xdata     TLB3ADDRH    _at_   0x3F39;   // TLB3 Address High Byte
volatile unsigned char xdata     TLB3ADDRL    _at_   0x3F3A;   // TLB3 Address Low Byte
volatile unsigned char xdata     TLB4INFOH    _at_   0x3F3B;   // TLB4 Information High Byte
volatile unsigned char xdata     TLB4INFOL    _at_   0x3F3C;   // TLB4 Information Low Byte
volatile unsigned char xdata     TLB4LENH     _at_   0x3F3D;   // TLB4 Length High Byte
volatile unsigned char xdata     TLB4LENL     _at_   0x3F3E;   // TLB4 Length Low Byte
volatile unsigned char xdata     TLB4ADDRH    _at_   0x3F3F;   // TLB4 Address High Byte
volatile unsigned char xdata     TLB4ADDRL    _at_   0x3F40;   // TLB4 Address Low Byte
volatile unsigned char xdata     TLB5INFOH    _at_   0x3F41;   // TLB5 Information High Byte
volatile unsigned char xdata     TLB5INFOL    _at_   0x3F42;   // TLB5 Information Low Byte
volatile unsigned char xdata     TLB5LENH     _at_   0x3F43;   // TLB5 Length High Byte
volatile unsigned char xdata     TLB5LENL     _at_   0x3F44;   // TLB5 Length Low Byte
volatile unsigned char xdata     TLB5ADDRH    _at_   0x3F45;   // TLB5 Address High Byte
volatile unsigned char xdata     TLB5ADDRL    _at_   0x3F46;   // TLB5 Address Low Byte
volatile unsigned char xdata     TLB6INFOH    _at_   0x3F47;   // TLB6 Information High Byte
volatile unsigned char xdata     TLB6INFOL    _at_   0x3F48;   // TLB6 Information Low Byte
volatile unsigned char xdata     TLB6LENH     _at_   0x3F49;   // TLB6 Length High Byte
volatile unsigned char xdata     TLB6LENL     _at_   0x3F4A;   // TLB6 Length Low Byte
volatile unsigned char xdata     TLB6ADDRH    _at_   0x3F4B;   // TLB6 Address High Byte
volatile unsigned char xdata     TLB6ADDRL    _at_   0x3F4C;   // TLB6 Address Low Byte
volatile unsigned char xdata     TLB7INFOH    _at_   0x3F4D;   // TLB7 Information High Byte
volatile unsigned char xdata     TLB7INFOL    _at_   0x3F4E;   // TLB7 Information Low Byte
volatile unsigned char xdata     TLB7LENH     _at_   0x3F4F;   // TLB7 Length High Byte
volatile unsigned char xdata     TLB7LENL     _at_   0x3F50;   // TLB7 Length Low Byte
volatile unsigned char xdata     TLB7ADDRH    _at_   0x3F51;   // TLB7 Address High Byte
volatile unsigned char xdata     TLB7ADDRL    _at_   0x3F52;   // TLB7 Address Low Byte
volatile unsigned char xdata     RXFIFOHEADH  _at_   0x3F17;   // Receive Buffer Head Pointer High Byte
volatile unsigned char xdata     RXFIFOHEADL  _at_   0x3F18;   // Receive Buffer Head Pointer Low Byte
volatile unsigned char xdata     RXFIFOTAILH  _at_   0x3F15;   // Receive Buffer Tail Pointer High Byte
volatile unsigned char xdata     RXFIFOTAILL  _at_   0x3F16;   // Receive Buffer Tail Pointer Low Byte
volatile unsigned char xdata     RXFIFOSTA    _at_   0x3F5B;   // Receive Buffer Status
volatile unsigned char xdata     TXSTARTH     _at_   0x3F59;   // Transmit Data Starting Address High Byte
volatile unsigned char xdata     TXSTARTL     _at_   0x3F5A;   // Transmit Data Starting Address Low Byte
volatile unsigned char xdata     TXAUTOWR     _at_   0x3F03;   // Transmit Data Autowrite
volatile unsigned char xdata     TXENDH       _at_   0x3F57;   // Transmit Data Ending Address High Byte
volatile unsigned char xdata     TXENDL       _at_   0x3F58;   // Transmit Data Ending Address Low Byte
volatile unsigned char xdata     TXCN         _at_   0x3F53;   // Transmit Control
volatile unsigned char xdata     TXPAUSEH     _at_   0x3F55;   // Transmit Pause High Byte
volatile unsigned char xdata     TXPAUSEL     _at_   0x3F56;   // Transmit Pause Low Byte
volatile unsigned char xdata     TXBUSY       _at_   0x3F54;   // Transmit Busy Indicator
volatile unsigned char xdata     TXSTA6       _at_   0x3F5C;   // Transmit Status Vector 6
volatile unsigned char xdata     TXSTA5       _at_   0x3F5D;   // Transmit Status Vector 5
volatile unsigned char xdata     TXSTA4       _at_   0x3F5E;   // Transmit Status Vector 4
volatile unsigned char xdata     TXSTA3       _at_   0x3F5F;   // Transmit Status Vector 3
volatile unsigned char xdata     TXSTA2       _at_   0x3F60;   // Transmit Status Vector 2
volatile unsigned char xdata     TXSTA1       _at_   0x3F61;   // Transmit Status Vector 1
volatile unsigned char xdata     TXSTA0       _at_   0x3F62;   // Transmit Status Vector 0
volatile unsigned char xdata     PHYCN        _at_   0x3F78;   // Physical Layer Control
volatile unsigned char xdata     PHYCF        _at_   0x3F79;   // Physical Layer Configuration
volatile unsigned char xdata     PHYSTA       _at_   0x3F80;   // Physical Layer Status
volatile unsigned char xdata     TXPWR        _at_   0x3F7A;   // Transmitter Power Register

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------