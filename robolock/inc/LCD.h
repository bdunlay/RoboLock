//Example LCD Initialization Program 
/*****************************************************/ 
void I2C_out(unsigned char j)    //I2C Output 
{ 
 int n; 
 unsigned char d; 
 d=j; 
 for(n=0;n<8;n++){ 
  if((d&0x80)==0x80) 
  SDA=1; 
  else 
  SDA=0; 
  d=(d<<1); 
  SCL = 0; 
  SCL = 1; 
  SCL = 0; 
  } 
 SCL = 1; 
 while(SDA==1){ 
  SCL=0; 
  SCL=1; 
  } 
 SCL=0; 
} 
/*****************************************************/ 
void I2C_Start(void) 
{ 
 SCL=1; 
 SDA=1; 
 SDA=0; 
 SCL=0; 
} 
/*****************************************************/ 
void I2C_Stop(void) 
{ 
 SDA=0; 
 SCL=0; 
 SCL=1; 
 SDA=1; 
} 
/*****************************************************/ 
void Show(unsigned char *text) 
{ 
 int n; 
 I2C_Start(); 
 I2C_out(Slave); 
 I2C_out(Datasend); 
 for(n=0;n<16;n++){ 
  I2C_out(*text); 
  ++text; 
  } 
 I2C_Stop(); 
} 
/**************************************************** [11] 
*           Initialization For ST7032i              * 
*****************************************************/ 
void init_LCD()  
{ 
I2C_Start(); 
I2C_out(0x7C); 
I2C_out(0x00); 
I2C_out(0x38); 
delay(10); 
I2C_out(0x39); 
delay(10); 
I2C_out(0x14); 
I2C_out(0x78); 
I2C_out(0x5E); 
I2C_out(0x6D); 
I2C_out(0x0C); 
I2C_out(0x01); 
I2C_out(0x06); 
delay(10); 
I2C_stop(); 
} 
/*****************************************************/
