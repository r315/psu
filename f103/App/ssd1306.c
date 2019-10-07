
#include "ssd1306.h"
#include "board.h"

typedef struct _Frame{
  uint8_t control;
  uint8_t data[SSD1306_LCDHEIGHT * SSD1306_LCDWIDTH / 8];
}Frame;

static Frame frame;

uint8_t *LCD_GetPixels(void){
  return frame.data;
}

void ssd1306_waitPowerUp(void){
uint8_t data[2];
 	data[0] = 0x00;   // Co = 0, D/C = 0
	data[1] = SSD1306_NOP;
    
	//while(HAL_I2C_Master_Transmit(&hi2c2, SSD1306_I2C_ADDRESS << 1, data, 2, 100) != HAL_OK){
  while(I2C_Write(SSD1306_I2C_ADDRESS, data, 2)){
    HAL_Delay(200);
  }   
}

void ssd1306_command(uint8_t c) {
uint8_t data[2];

	data[0] = 0x00;   // Co = 0, D/C = 0
	data[1] = c;

  I2C_Write(SSD1306_I2C_ADDRESS, data, 2);  
	//HAL_I2C_Master_Transmit(&hi2c2, SSD1306_I2C_ADDRESS << 1, data, 2, 100);
}

// startscrollright
// Activate a right handed scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F)
void LCD_ScrollRight(uint8_t start, uint8_t stop){
  ssd1306_command(SSD1306_RIGHT_HORIZONTAL_SCROLL);
  ssd1306_command(0X00);
  ssd1306_command(start);
  ssd1306_command(0X00);
  ssd1306_command(stop);
  ssd1306_command(0X00);
  ssd1306_command(0XFF);
  ssd1306_command(SSD1306_ACTIVATE_SCROLL);
}

void LCD_StopScroll(void){
  ssd1306_command(SSD1306_DEACTIVATE_SCROLL);
}


void LCD_Init(void){
#if defined(USE_I2C_DMA)  
  i2cCfgDMA((uint8_t*)&frame, sizeof(Frame));
#endif  
  ssd1306_waitPowerUp();

  ssd1306_command(SSD1306_SETMULTIPLEX);                  // 0xA8
  
  ssd1306_command(SSD1306_LCDHEIGHT - 1);

  ssd1306_command(SSD1306_SETDISPLAYOFFSET);              // 0xD3
  ssd1306_command(0x0);                                   // no offset

  ssd1306_command(SSD1306_SETSTARTLINE | 0x0);            // line #0

  ssd1306_command(SSD1306_SEGREMAP | 0x1);
  
  ssd1306_command(SSD1306_COMSCANDEC);

  ssd1306_command(SSD1306_SETCOMPINS);                    // 0xDA
  ssd1306_command(0x02);  
  
  ssd1306_command(SSD1306_SETCONTRAST);                   // 0x81
  ssd1306_command(0x8F);

  ssd1306_command(SSD1306_DISPLAYALLON_RESUME);           // 0xA4

  ssd1306_command(SSD1306_NORMALDISPLAY);                 // 0xA6
  
  ssd1306_command(SSD1306_SETDISPLAYCLOCKDIV);            // 0xD5
  ssd1306_command(0x80);                                  // the suggested ratio 0x80

  ssd1306_command(SSD1306_CHARGEPUMP);                    // 0x8D
  ssd1306_command(0x14);
  
  ssd1306_command(SSD1306_DISPLAYON);

  ssd1306_command(SSD1306_MEMORYMODE);                    // 0x20
  ssd1306_command(0x00);                                  // 0x0 act like ks0108  
  
  //ssd1306_command(SSD1306_DEACTIVATE_SCROLL);  
  //ssd1306_command(SSD1306_SETPRECHARGE);                  // 0xd9
  //ssd1306_command(0xF1);
  //ssd1306_command(SSD1306_SETVCOMDETECT);                 // 0xDB
  //ssd1306_command(0x40);  
}


void LCD_Pixel(uint16_t x, uint16_t y, uint16_t color) {
  if (x >= SSD1306_LCDWIDTH || y >= SSD1306_LCDHEIGHT)
    return;
/*
  // check rotation, move pixel around if necessary
  switch (getRotation()) {
  case 1:
    ssd1306_swap(x, y);
    x = WIDTH - x - 1;
    break;
  case 2:
    x = WIDTH - x - 1;
    y = HEIGHT - y - 1;
    break;
  case 3:
    ssd1306_swap(x, y);
    y = HEIGHT - y - 1;
    break;
  }
*/
  // x is which column
    switch (color)
    {
      case WHITE:   frame.data[x+ (y/8)*SSD1306_LCDWIDTH] |=  (1 << (y&7)); break;
      case BLACK:   frame.data[x+ (y/8)*SSD1306_LCDWIDTH] &= ~(1 << (y&7)); break;
      case INVERSE: frame.data[x+ (y/8)*SSD1306_LCDWIDTH] ^=  (1 << (y&7)); break;
    }
}

uint16_t LCD_GetWidth(void){
	return SSD1306_LCDWIDTH;
}

uint16_t LCD_GetHeight(void){
	return SSD1306_LCDHEIGHT;
}

void LCD_Update(void) {
  ssd1306_command(SSD1306_COLUMNADDR);
  ssd1306_command(0);   // Column start address (0 = reset)
  ssd1306_command(SSD1306_LCDWIDTH-1); // Column end address (127 = reset)

  ssd1306_command(SSD1306_PAGEADDR);
  ssd1306_command(0); // Page start address (0 = reset)  
  ssd1306_command(3); // Page end address
  
  frame.control = 0x40;
#if defined(USE_I2C_DMA)  
  I2C_WriteDMA(SSD1306_I2C_ADDRESS, (uint8_t*)&frame, sizeof(frame));
#else
  I2C_Write(SSD1306_I2C_ADDRESS, (uint8_t*)&frame, sizeof(frame));
#endif
  //HAL_I2C_Master_Transmit(&hi2c2, SSD1306_I2C_ADDRESS << 1, (uint8_t*)&frame, sizeof(frame), 500);  
}

void LCD_Fill(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color){
  for(uint16_t i = y; i < y + h; i++){
    for(uint16_t j = x; j < x + w; j++){
      if(color)
        frame.data[j + (i/8)*SSD1306_LCDWIDTH] |= (1 << (i&7));
      else
        frame.data[j + (i/8)*SSD1306_LCDWIDTH] &= ~(1 << (i&7));
    }    
  }
}

void LCD_SetFrame(uint16_t c, uint16_t p, uint8_t *data){
  uint16_t size = *data++;
  uint8_t *dst = frame.data + (p * SSD1306_LCDWIDTH) + c;
  while(size--){
    *dst++ = *data++;
  }
}