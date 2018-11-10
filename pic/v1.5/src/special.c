//------------------------------------------------------
// convert integer to array of decimal
//-----------------------------------------------------
void intToString(char* dst, unsigned int value, char radix)
{
 char k, i=0, tmp[8];
 
	do{
		k = (char)(value % radix);
		if(k > 9)
			k += 'A';
		else
			k += '0';
		value /= radix;			
		tmp[i++] = k;		
	}while(value != 0);		
	
	while(i--)
		*dst++ = tmp[i];	
}
