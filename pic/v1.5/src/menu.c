extern char setvout, setiout, menu,update;
//------------------------------------------------------
//
//------------------------------------------------------
void drawMenu(void)
{	
	setFontPatern(UNDERLINE);
	printString(6,7,"                   ");
	setFontPatern(INVERTED);
	switch(menu)
	{
		case VMENU:
			printString(6,7," Volts ");			
			break;
		case AMENU:			
			printString(6,7," Amps ");
			break;
		case LMENU:
			printString(6,7," Load ");
			break;
	}	
}
//------------------------------
//
//------------------------------
void selectMode(void)
{
char imenu = menu;	

	stopCounting();
	LOAD_EN = 0;
	
	setFontPatern(UNDERLINE);
	printString(6,7," Volts  Amps  Load ");

	while(scanKeys())DelayMs(10);
	setFontPatern(INVERTED);	
	
	while(1)
	{
		if(keydown(L_KEY))
		{
			menu = VMENU;			
			printString(6,7," Volts ");
			break;
		}
		if(keydown(M_KEY))
		{
			menu = AMENU;			
			printString(6+(7*6),7," Amps ");
			break;
		}
		if(keydown(R_KEY))
		{
			menu = LMENU;			
			printString(6+(13*6),7," Load ");
			setiout = 0;
			LOAD_EN = 1;
			setDuty(ISET_CH,setiout);
			startCounting();
			break;
		}
	}
	
	if(menu != imenu) // optimização para evitar desenhar as frames sempre
	{
		if(imenu == LMENU && (menu == AMENU || menu == VMENU))		
			psFrame();					
		if(menu == LMENU && (imenu == AMENU || imenu == VMENU))
			loadFrame();					
	}
	DelayMs(250);
	while(scanKeys())DelayMs(10);
	drawMenu();
}
//------------------------------
//
//------------------------------
void changeUp(void)
{
	switch(menu)
	{
		case VMENU:
			if(setvout < MAXVOUT)
				setDuty(VSET_CH,++setvout);
			break;		
		case AMENU:
		case LMENU:
			if(setiout < MAXIOUT)
				setDuty(ISET_CH,++setiout);
			break;
		default: break;		
	}			
}
//------------------------------
//
//------------------------------
void changeDown(void)
{
	switch(menu)
	{
		case VMENU:
			if(setvout > MINVOUT)
				setDuty(VSET_CH,--setvout);
			break;
		case AMENU:
		case LMENU:
			if(setiout > MINIOUT)
				setDuty(ISET_CH,--setiout);
			break;
		default: break;		
	}			
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
void butoes(void)
{
	if(keydown(L_KEY)){
		changeDown();
		update = 20;
	}
	
	if(keydown(R_KEY)){
		changeUp();
		update = 20;
	}
	
	if(keydown(M_KEY)){		
		selectMode();		
	}
}


