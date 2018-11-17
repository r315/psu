#ifndef _vcom_h_
#define _vcom_h_


#ifdef __cplusplus
extern "C" {
#endif

class Vcom{

public:  
    
    uint8_t checkForChar(char *c);
    void init(void);
	char getchar(void);
	void putchar(char c);
    void puts(const char* str);
    char getCharNonBlocking(char *c);
    Vcom(void) {init();}
};


#ifdef __cplusplus
}
#endif

#endif