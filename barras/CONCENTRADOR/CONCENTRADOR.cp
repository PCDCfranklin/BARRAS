#line 1 "D:/VICENTE/Downloads/PC/ALGORITMOS_CODIGOS/GIT_GITHUB/BARRAS/barras/CONCENTRADOR/CONCENTRADOR.c"









sbit Stx0_pin at PORTB.B1;
sbit Srx0_pin at PORTB.B2;
sbit Scts0_pin at Stx0_pin;
sbit Stx0_pin_Direction at TRISB.B1;
sbit Srx0_pin_Direction at TRISB.B2;
sbit Scts0_pin_Direction at Stx0_pin_Direction;


sbit Stx2_pin at PORTB.B6;
sbit Srx2_pin at PORTB.B7;
sbit Scts2_pin at Stx2_pin;
sbit Stx2_pin_Direction at TRISB.B6;
sbit Srx2_pin_Direction at TRISB.B7;
sbit Scts2_pin_Direction at Stx2_pin_Direction;


sbit rs485_rxtx_pin at RA2_bit;
sbit rs485_rxtx_pin_direction at TRISA2_bit;
char master_rx_dat[7]={0,0,0,0,0,0,0};
char master_tx_dat[6]={0,0,0,0,0,0};
char buffer[50],s_entran[11],s_salen[11],s_bloqueos[11];
unsigned long int entran=0,salen=0,bloqueos=0 , cnt=0;
char fbt=0, pbuffer=0, u=0, id_slave=0;


char dat[10];
char i,j;
char esclavo = 10;
unsigned long int seg_off = 0;
int sinRespuesta = 0;
char response[11];
char esclavo_ant = 0;
unsigned long anterior = 0, actual = 0, cnt1 = 0, cnt2 = 0;
unsigned long sinE1 = 0, sinE2 = 0, sinE3 = 0;
char vandalismo = 0x41;
int suma = 0, reset = 0;
unsigned long controlGPS = 0;



unsigned long int counter1=0, counter2=0;
short int ax=0;
unsigned short int ee1[24]={'r','a','s','e','r','c','o','m','.','R','S','C',',','1','0','0','0','0','0','0','0',',',13,10};
unsigned short int ee2[24]={'r','a','s','e','r','c','o','m','.','R','S','C',',','2','0','0','0','0','0','0','0',',',13,10};
unsigned short int ee3[24]={'r','a','s','e','r','c','o','m','.','R','S','C',',','3','0','0','0','0','0','0','0',',',13,10};



void buildBuf600();
void imprimirAlerta(char lugar);
void imprimirMensaje(char mensaje[10]);
void peticion(char dirEsclavo);
void reiniciarEsclavos();
void restaurarEsclavos();
void transmitirGPS(int GPS);



void interrupt()
{
 RS485Master_Receive(master_rx_dat);
}

void main()
{
 ADCON1= 0b00001111;
 CMCON = 0b00000111;
 TRISA.RA3=0; TRISA.RA4=0;
 PORTA.RA3=0; PORTA.RA4=0;


 SUart0_Init_T();
 SUart2_Init_T();

 TRISC.RC0 = 1;
 PORTC.RC0 = 0;
 TRISB.RB5 = 0;
 PORTB.RB5 = 0;

 TRISC.TRISC1 = 1;
 PORTC.RC1 = 1;
 TRISC.TRISC2 = 1;
 PORTC.RC2 = 1;


 UART1_Init(9600); Delay_ms(100);
 RS485Master_Init(); Delay_ms(100);
 RCIE_bit = 1;
 TXIE_bit = 0;
 PEIE_bit = 1;
 GIE_bit = 1;


 peticion(esclavo);

 while(1)
 {

 if (master_rx_dat[5])
 {
  PORTA.RA4 =1;
 Delay_ms(10);
  PORTA.RA4 =0;
 master_rx_dat[5]=0;
 master_rx_dat[4]=0;
 }


 if(fbt>0)
 {
 cnt++;
 }
 if(cnt>14000*1)
 {
 cnt=0;
 fbt=0;
  PORTA.RA3 =0;
 pbuffer=0;
 entran=0; salen=0; bloqueos=0;

 imprimirAlerta('R');
 }


 if (master_rx_dat[4] && !master_rx_dat[5])
 {
 if(fbt==0)
 {
 cnt=0;
 entran=0;
  PORTA.RA3 =1;
 buffer[pbuffer++]='i';
 id_slave=master_rx_dat[6];
 buffer[pbuffer++]=master_rx_dat[6]+48;
 entran+=(unsigned long int)master_rx_dat[0];
 entran+=(((unsigned long int)master_rx_dat[1])<<8);
 entran+=(((unsigned long int)master_rx_dat[2])<<16);
 fbt=1;
 }
 else if(fbt==1)
 {
 entran+=(((unsigned long int)master_rx_dat[0])<<24);
 salen+=(unsigned long int)master_rx_dat[1];
 salen+=(((unsigned long int)master_rx_dat[2])<<8);
 fbt=2;
 }
 else if(fbt==2)
 {
 salen+=(((unsigned long int)master_rx_dat[0])<<16);
 salen+=(((unsigned long int)master_rx_dat[1])<<24);
 bloqueos+=(unsigned long int)master_rx_dat[2];
 fbt=3;
 }
 else if(fbt==3)
 {
 bloqueos+=(((unsigned long int)master_rx_dat[1])<<8);
 bloqueos+=(((unsigned long int)master_rx_dat[2])<<16);
 bloqueos+=(((unsigned long int)master_rx_dat[1])<<24);
 fbt=4;
 }
 master_rx_dat[4] = 0; master_rx_dat[6]=0;
 }

 if(fbt==4)
 {
  PORTA.RA3 =0;
 LongWordToStrWithZeros(entran,s_entran);
 LongWordToStrWithZeros(salen,s_salen);
 LongWordToStrWithZeros(bloqueos,s_bloqueos);
 buffer[pbuffer++]='E';
 for(u=0;u<10;u++){ buffer[pbuffer++]=s_entran[u]; }
 buffer[pbuffer++]='S';
 for(u=0;u<10;u++){ buffer[pbuffer++]=s_salen[u]; }
 buffer[pbuffer++]='B';
 for(u=0;u<10;u++){ buffer[pbuffer++]=s_bloqueos[u]; }
 buffer[pbuffer++]='V';
 buffer[pbuffer++]=vandalismo;
 buffer[pbuffer++]='#';

 SUart0_RstrNout(buffer,38);
 SUart0_write('\r'); SUart0_write('\n');




 transmitirGPS(300);

 fbt=0; pbuffer=0;

 entran=0; salen=0; bloqueos=0;
 cnt=0;


 cnt1 = cnt2 = 0;
 suma = 0;
 reset = 1;

 }
 else
 {
 cnt1++;
 if(cnt1 > 140000)
 {
 if(esclavo == 10)
 esclavo_ant = 30;
 else if(esclavo == 20)
 esclavo_ant = 10;
 else if(esclavo == 30)
 esclavo_ant = 20;
#line 233 "D:/VICENTE/Downloads/PC/ALGORITMOS_CODIGOS/GIT_GITHUB/BARRAS/barras/CONCENTRADOR/CONCENTRADOR.c"
 cnt1 = 0;
 cnt2++;

 if(cnt2 > 9)
 {
 if(esclavo_ant == 10)
 {
 sinE1++;
 if(sinE1 > 4)
 {
 vandalismo.B1 = 1;
 response[0]='F';response[1]='A';response[2]='L';
 response[3]='L';response[4]='A';response[5]=' ';
 response[6]='E';response[7]='N';response[8]=' ';
 response[9]='S';response[10]=((esclavo_ant/10)+48);
 imprimirMensaje(&response);
 if(sinE1 > 9)
 sinE1 = 5;
 }
 }
 else if(esclavo_ant == 20)
 {
 sinE2++;
 if(sinE2 > 4)
 {
 vandalismo.B2 = 1;
 response[0]='F';response[1]='A';response[2]='L';
 response[3]='L';response[4]='A';response[5]=' ';
 response[6]='E';response[7]='N';response[8]=' ';
 response[9]='S';response[10]=((esclavo_ant/10)+48);
 imprimirMensaje(&response);
 if(sinE2 > 9)
 sinE2 = 5;
 }
 }
 else if(esclavo_ant == 30)
 {
 sinE3++;
 if(sinE3 > 4)
 {
 vandalismo.B3 = 1;
 response[0]='F';response[1]='A';response[2]='L';
 response[3]='L';response[4]='A';response[5]=' ';
 response[6]='E';response[7]='N';response[8]=' ';
 response[9]='S';response[10]=((esclavo_ant/10)+48);
 imprimirMensaje(&response);
 if(sinE3 > 9)
 sinE3 = 5;
 }
 }
 cnt2=0;
 }
 else if(reset)
 {
 if(esclavo_ant == 10)
 {
 sinE1 = 0;
 vandalismo.B1 = 0;





 }
 else if(esclavo_ant == 20)
 {
 sinE2 = 0;
 vandalismo.B2 = 0;





 }
 else if(esclavo_ant == 30)
 {
 sinE3 = 0;
 vandalismo.B3 = 0;





 }
 reset = 0;
 }
 }
 }
#line 331 "D:/VICENTE/Downloads/PC/ALGORITMOS_CODIGOS/GIT_GITHUB/BARRAS/barras/CONCENTRADOR/CONCENTRADOR.c"
 counter2++;
 if(counter2>(140000*10))
 {
 counter2=0;




 response[0] = 'B';response[1] = 'A';
 response[2] = 'R';response[3] = 'R';
 response[4] = 'A';response[5] = ' ';
 response[6] = ' ';response[7] = ' ';
 response[8] = '>';response[9] = ' ';response[10] = ((esclavo/10)+48);
 imprimirMensaje(&response);
 peticion(esclavo);
 esclavo += 10;
 if(esclavo > 30){esclavo = 10;}
 }
#line 356 "D:/VICENTE/Downloads/PC/ALGORITMOS_CODIGOS/GIT_GITHUB/BARRAS/barras/CONCENTRADOR/CONCENTRADOR.c"
 counter1++;
 if(counter1>(140000*10))
 {
 counter1=0;

 }
#line 368 "D:/VICENTE/Downloads/PC/ALGORITMOS_CODIGOS/GIT_GITHUB/BARRAS/barras/CONCENTRADOR/CONCENTRADOR.c"
 if( PORTC.RC0 )
 {
 seg_off++;




 if((seg_off > (54026 * 10)) &&  PORTB.RB5  == 0)
 {
 seg_off = 0;
  PORTB.RB5  = 1;
 SUart0_write('O'); SUart0_write('F'); SUart0_write('F');
 SUart0_write('\r');
 SUart0_write('\n');
 }

 }
 else
 {
 seg_off = 0;
  PORTB.RB5  = 0;
 }
#line 425 "D:/VICENTE/Downloads/PC/ALGORITMOS_CODIGOS/GIT_GITHUB/BARRAS/barras/CONCENTRADOR/CONCENTRADOR.c"
 }
}
#line 434 "D:/VICENTE/Downloads/PC/ALGORITMOS_CODIGOS/GIT_GITHUB/BARRAS/barras/CONCENTRADOR/CONCENTRADOR.c"
void imprimirAlerta(char lugar)
{
 SUart0_write(lugar);
 SUart0_write('\r');
 SUart0_write('\n');
}
void imprimirMensaje(char mensaje[11])
{
 int u = 0;
 for(u = 0; u < 11; u++)
 {
 SUart0_write(mensaje[u]);
 }
 SUart0_write('\r');
 SUart0_write('\n');
}
#line 461 "D:/VICENTE/Downloads/PC/ALGORITMOS_CODIGOS/GIT_GITHUB/BARRAS/barras/CONCENTRADOR/CONCENTRADOR.c"
void peticion(char dirEsclavo)
{
 dat[0] = 0xFF;
 dat[1] = 0xFF;
 dat[2] = 0xFF;
 dat[4] = 0;
 dat[5] = 0;
 dat[6] = 0;
 RS485Master_Send(dat,1,dirEsclavo);
 delay_ms(1);
}
#line 483 "D:/VICENTE/Downloads/PC/ALGORITMOS_CODIGOS/GIT_GITHUB/BARRAS/barras/CONCENTRADOR/CONCENTRADOR.c"
void reiniciarEsclavos()
{
 dat[0] = 0xFA;
 dat[1] = 0xFA;
 dat[2] = 0xFA;
 dat[4] = 0;
 dat[5] = 0;
 dat[6] = 0;
 RS485Master_Send(dat,1,50);
 delay_ms(10);
}
#line 504 "D:/VICENTE/Downloads/PC/ALGORITMOS_CODIGOS/GIT_GITHUB/BARRAS/barras/CONCENTRADOR/CONCENTRADOR.c"
void restaurarEsclavos()
{
 dat[0] = 0xFB;
 dat[1] = 0xFB;
 dat[2] = 0xFB;
 dat[4] = 0;
 dat[5] = 0;
 dat[6] = 0;
 RS485Master_Send(dat,1,50);
 delay_ms(10);
}
#line 523 "D:/VICENTE/Downloads/PC/ALGORITMOS_CODIGOS/GIT_GITHUB/BARRAS/barras/CONCENTRADOR/CONCENTRADOR.c"
void buildBuf600()
{
 if(id_slave == 10)
 {
 for(u=3;u<10;u++){ ee1[11+u]=s_entran[u]; }
 }
 if(id_slave == 20)
 {
 for(u=3;u<10;u++){ ee2[11+u]=s_entran[u]; }
 }
 if(id_slave == 30)
 {
 for(u=3;u<10;u++){ ee3[11+u]=s_entran[u]; }
 }
}
#line 550 "D:/VICENTE/Downloads/PC/ALGORITMOS_CODIGOS/GIT_GITHUB/BARRAS/barras/CONCENTRADOR/CONCENTRADOR.c"
void transmitirGPS(int GPS)
{
 if(GPS == 300)
 {
 for(u=0;u<38;u++)
 {
 Suart2_write((char)buffer[u]);
 }
 }

 else if(GPS == 600)
 {
 if(ax==0)
 {
 ax = 1;
 for(u=0;u<24;u++)
 {
 Suart2_write((char)ee1[u]);
 }
 }
 else if (ax == 1)
 {
 ax = 2;
 for(u=0;u<24;u++)
 {
 Suart2_write((char)ee2[u]);
 }
 }
 else if (ax == 2)
 {
 ax = 0;
 for(u=0;u<24;u++)
 {
 Suart2_write((char)ee3[u]);
 }
 }
 }
}
