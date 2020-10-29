#include "cmdutil.h"

char *getOptValue(const char *opt, uint32_t argc, char **argv){
    for(uint32_t i = 0; i < argc; i++){
        if(xstrcmp(opt, argv[i]) == 0){
            return argv[i+1];
        }
    }
    return NULL;
}

int readIntParameter(const char *opt, uint32_t argc, char **argv, uint16_t *dst, void (*func)(uint32_t)){
    char *param = getOptValue(opt, argc, argv);
    int32_t i;
    
    if(param == NULL){
        return CMD_NOT_FOUND;
    }		

    if(nextInt(&param, &i) == 0){
        return CMD_BAD_PARAM;
    }

    func(i);

    return CMD_OK;	
}

int readFloatParameter(const char *opt, uint32_t argc, char **argv, uint16_t *dst, void (*func)(float)){
    union {float f; uint32_t u; } t;

    char *param = getOptValue((char*)opt, argc, argv);
    double d;
    
    if(param == NULL){
        return CMD_NOT_FOUND;
    }		

    if(nextDouble(&param, &d) == 0){
        return CMD_BAD_PARAM;
    }

    t.f = d;		
    *dst = (uint16_t)t.u;
    *(dst + 1) = (uint16_t)(t.u>>16);
    func(t.f);

    return CMD_OK;	
}


/*
void dummy(float val){
    DBG_PRINT("%s: %f\n", __FUNCTION__, val);
}
*/

