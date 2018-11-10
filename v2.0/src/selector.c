#include "common.h"


struct MenuItem *changeMode(struct MenuItem *mods, char nmods){
	return selectMenuItem(mods,nmods);
}
