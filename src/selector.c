#include "common.h"


struct MenuItem *changeMode(struct MenuItem *mods, char nmods){
#define MODS_SPACING 5*FONT_W
	return selectMenuItem(mods,nmods,MODS_SPACING);
}
