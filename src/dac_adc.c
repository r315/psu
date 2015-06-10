
//--------------------------------------------
// returns V in mV
//--------------------------------------------
int getVout(char ch)
{	
	return  ADIN(ch)*VCONST;
}
//--------------------------------------------
// returns I in mA
//--------------------------------------------
int getIout(void)
{
	return ADIN(IOUT_CH)*ICONST;
}





