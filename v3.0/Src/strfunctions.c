
#include "strfunctions.h"

#define FLOAT_MAX_PRECISION 6

size_t strlen(const char *str){
size_t count = 0;
	while(*str++){
		count++;
	}
	return count;	
}

char * strchr ( const char *str, int c){
	while(*str){
		if(*str == c)
			break;
		str += 1;
	}
	return (char*)str;
}

char *nextParameter(char *line){
    line = strchr(line, ' ');
    if(*line != '\0')
        line++;
    return line;
}

uint32_t nextHex(char **line){
uint32_t hex;    
    hex = hatoi(*line);
    *line = nextParameter(*line);
    return hex;
}

int32_t nextInt(char **line){
int32_t rint;    
    rint = yatoi(*line);
    *line = nextParameter(*line);
    return rint;
}

char nextChar(char **line){
char c;    
    c = *line[0];
    *line = nextParameter(*line);
    return c;
}


/**
 * get first occuring substring from a token split of a given string
 * 
 * @param str:		reference to input string, on return the reference will pointer 
 * 					for then remaining string. 
 * @param token:	token to split the string
 * @param len:		length of the given string
 * @param saveptr:	pointer for saving the remaining string
 * 
 * returns: 		pointer for found sub string or nul if not splitted
 **/

char *strtok_s(char *str, const char token, uint8_t len, char **saveptr){
char *ptr, i = 0;	

	if(str == NULL){
		return NULL;
	}

	// save pointer for return
	ptr = str;

	// search token, if finds it 
	// replace it with string terminator character
	while(*ptr && i < len){
		if(*ptr == token){
			*ptr = '\0';  
			break;
		}
		ptr += 1;
		i++;
	}

	// got to the end of the string and token was not found return null
	// or string is empty
    if ( i == len || ptr == str){
        return NULL;
	}

	

	// if specified, return pointer to remaining string
	if( saveptr != NULL ){
		*saveptr = ptr + 1;
	}

	// return pointer to substring
	return str;
}

char xstrcmp(char const *str1, char const *str2){
	do{
		if(*str1 != *str2)
			return (*str1 - *str2);		
	}while(*(++str1) && *(++str2));
	return 0;
}

// convert decimal str to integer
int yatoi(char *str){
int val=0;	
char c, radix = 10, s = 0;
	if(*str == '-'){
		s = 1;
		str++;
	}
	while(*str){
		c = *str;
		if(c < '0' || c > '9'){
			if(c == ' ' || c == '\n'){
				break;
			}
			return 0;
		}
		c -= '0';
		val = val * radix + c;
		str +=1;
	}
	return s? -val : val;
}

#if 0
// convert str to integer seams not working
int hatoi(char *str){
int val = -1;	
char c;
	while(*str){		
		c = (*str) - ('a'+'A');
		if(c > '/' && c < ':')
			c -= '0';
		else if((c >'@' && c < 'G'))
			c -= 'A';
		else
			return -1;			
		val |= c;
		val <<= 4;
		str +=1;
	}
	return val;
}
#endif

// convert hex str to integer
uint32_t hatoi(char *str){
uint32_t val = 0;	
char c = *str;
	do{
        val <<= 4;
        if(c > '`' && c < 'g'){
            c -= 'W'; 
        }else if((c >'@' && c < 'G')){
            c -= '7';
        }else if(c > '/' && c < ':'){
			c -= '0';
        }else{
			return -1;
		}

		val |= c;		
		c = *(++str);
        
	}while( c != '\0' && c != ' ' && c != '\n' );

	return val;
}


/* Original code by ELM_ChaN. Modified by Martin Thomas */
int xatoi (char **str, long *res)
{
	uint32_t val;
	uint8_t c, radix, s = 0;


	while ((c = **str) == ' ') (*str)++;
	if (c == '-') {
		s = 1;
		c = *(++(*str));
	}
	if (c == '0') {
		c = *(++(*str));
		if (c <= ' ') {
			*res = 0; return 1;
		}
		if (c == 'x') {
			radix = 16;
			c = *(++(*str));
		} else {
			if (c == 'b') {
				radix = 2;
				c = *(++(*str));
			} else {
				if ((c >= '0')&&(c <= '9'))
					radix = 8;
				else
					return 0;
			}
		}
	} else {
		if ((c < '1')||(c > '9'))
			return 0;
		radix = 10;
	}
	val = 0;
	while (c > ' ') {
		if (c >= 'a') c -= 0x20;
		c -= '0';
		if (c >= 17) {
			c -= 7;
			if (c <= 9) return 0;
		}
		if (c >= radix) return 0;
		val = val * radix + c;
		c = *(++(*str));
	}
	if (s) val = -val;
	*res = val;
	return 1;
}

/**
 * Convert integer to string
 * 
 * \param val		value to be converted
 * \param radix		base of convertion [-10,10,16]
 * \param len 		number of digits, len > 0 pad with ' ', len < 0 pad with '0' 
 * \return 			pointer to string
 * */
#define XTOA_BUF_SIZE 20
char* pitoa (long val, int radix, int len)
{
	static uint8_t s[XTOA_BUF_SIZE];
	uint8_t i, c, r, sgn = 0, pad = ' ';
	uint32_t v;

	if (radix < 0) {
		radix = -radix;
		if (val < 0) {
			val = -val;
			sgn = '-';
		}
	}

	v = val;
	r = radix;

	if (len < 0) {
		len = -len;
		pad = '0';
	}

	if (len > XTOA_BUF_SIZE){
		len = XTOA_BUF_SIZE;
	}		

	len = XTOA_BUF_SIZE - 1 - len;
	i = XTOA_BUF_SIZE;
	s[--i] = '\0';

	do {
		c = (uint8_t)(v % r);
		if (c >= 10) c += 7;
		c += '0';
		s[--i] = c;		
		v /= r;
	} while (v);

	if (sgn) s[--i] = sgn;

	while (i > len){
		s[--i] = pad;
	}

	return (char*)(s + i);
}

/**
 * Convert integer to string
 * 
 * https://en.wikipedia.org/wiki/Single-precision_floating-point_format
 * https://wirejungle.wordpress.com/2011/08/06/displaying-floating-point-numbers
 * 
 * \param f			value to be converted
 * \param places	number of decimal places
 * \return 			pointer to string
 * */
char *pftoa(double f, char places){
long int_part, frac_part;
char prec;
static char s[XTOA_BUF_SIZE], *pstr, *pdst;

    int_part = (long)(f);  
    
	if(places > FLOAT_MAX_PRECISION)
		places = FLOAT_MAX_PRECISION;
		
	frac_part = 0;
	prec = 0;

	while ((prec++) < places){
		f *= 10;
		frac_part = (frac_part * 10) + (long)f - ((long)f / 10) * 10;  //((long)f%10);			
	}

	pdst = s;
    pstr = pitoa(int_part, -10, 0);

	while(*pstr){
		*(pdst++) = (*pstr++);
	}
    
	*pdst++ = '.';

    pstr = pitoa(abs(frac_part), 10, -places); 

	do{
		*(pdst++) = (*pstr);
	}while(*pstr++);

	return s;
}

//-----------------------------------------------------------
//
//-----------------------------------------------------------
void * memcpy ( void * destination, const void * source, size_t num ){
	for(size_t i = 0; i < num; i++){
		*((uint8_t*)destination + i) = *((uint8_t*)source);
		source = (uint8_t*)source + 1; 
	}
	return destination;
}

void * memset ( void * ptr, int value, size_t num ){
	for(size_t i = 0; i < num; i++){
		*((uint8_t*)ptr + i) = (uint8_t)value;
	}
	return ptr;
}
