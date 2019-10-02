extern char setvout, setiout;
//--------------------------------------------
// returns V in mV
//--------------------------------------------
int getVout(char ch)
{
#ifdef DEBUG
	return setvout*VCONST;
#else	
	return  ADIN(ch)*VCONST;
#endif
}
//--------------------------------------------
// returns I in mA
//--------------------------------------------
int getIout(void)
{
#ifdef DEBUG
	return setiout*ICONST;
#else
	return ADIN(IOUT_CH)*ICONST;
#endif
}





