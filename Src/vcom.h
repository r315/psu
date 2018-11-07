#ifndef _vcom_h_
#define _vcom_h_


#ifdef __cplusplus
extern "C" {
#endif

class Vcom{
	
public:  
    // All calls are blocking
    void init(void);
	void putc(char c);
	char getc();
    void putString(const char *s, uint8_t len);
};


#ifdef __cplusplus
}
#endif

#endif