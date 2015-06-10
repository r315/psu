//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
void adcInit(void)
{
TRISA = 255;
// RCosc, ADON
ADCON0 = 0xC1;
// Right justified --1111111111
ADCON1 = 0x80;
// Left justified  1111111111--
//ADCON1 = 0x00;
}
//-----------------------------------------------------------------
// 
//-----------------------------------------------------------------
#if defined(_16F873) || defined(_16F819)
unsigned int ADIN(char ch)
{
	ADCON0 &=0xC1;		// clear current channel select	
	ADCON0 |= (ch<<3);	// apply the new channel select
	DelayUs(50);
	GODONE = 1;			// initiate conversion on the selected channel
	while(GODONE);		
return ADRESL | (ADRESH << 8);
}
#elif defined(_16F73)
unsigned char ADIN(char ch)
{	
	ADCON0 &=0xC1;		// clear current channel select	
	ADCON0 |= (ch<<3);	// apply the new channel select
	DelayUs(50);
	ADGO = 1;			// initiate conversion on the selected channel
	while(ADGO);
return ADRES;
}
#elif defined(_16F877)
unsigned int ADIN(char ch)
{
	ADCON0 &= 0xC1;		// clear current channel select	
	ADCON0 |= (ch<<3);	// apply the new channel select
	DelayUs(50);
	ADGO = 1;			// initiate conversion on the selected channel
	while(ADGO);		
return (ADRESH << 8) | ADRESL;
}
#endif
