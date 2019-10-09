
#include "psu.h"


/**
 * HW modules configuration
 * TIM4   program loop tick
 * TIM2   ADC Trigger
 * DMA1   ADC data transfer
 * ADC1-2 Dual configuration for simultaneous convertion
 * TIM3   PWM signals
 * */

static ModePsu cpsu;
static ModeLoad cload;
static Screen *screens[] = {
    &cpsu,
    &cload      
};


class VoltageDro : public ConsoleCommand{
    double v = 0;
public:
    char execute(void *ptr){
        //SEVEN_Double(1,1,v);
        v += 0.5;
        return CMD_OK;
    }
    void help(void){}

    VoltageDro(void) : ConsoleCommand("vdro") {}
};


class CmdOut : public ConsoleCommand {
	Console *console;
public:
    CmdOut() : ConsoleCommand("out") {}	
	void init(void *params) { console = static_cast<Console*>(params); }

	void help(void) {
	}

	char execute(void *ptr) {
		uint32_t pdata;
		if(!nextHex((char**)&ptr, &pdata)){
            console->print("usage: out <state>\n\tstate: 0 - off, 1 - on\n");
        	return CMD_BAD_PARAM;
        }

		if(pdata == 1){
			setOutput(1);
		}else{
			setOutput(0);
		}
		
		return CMD_OK;
	}	
};




void ModePsu::redraw(void){
    LCD_Fill(0, 0, LCD_W, LCD_H, BLACK);
    //TEXT_setFont(&defaultBoldFont);
    //TEXT_setFont(&lcdFont);
    TEXT_setFont(&pixelDustFont);
    //TEXT_setFont(&defaultFont);
    TEXT_print(0,0, "88.8W");
    TEXT_setFont(&font_seven_seg);
    TEXT_dro(VOLTAGE_DRO_POS, 0.00, 1);
    TEXT_dro(CURRENT_DRO_POS, 0.00, 2);
    TEXT_drawGfx(PSU_ICON_POS, (uint8_t*)&icon_psu[0]);
}

void ModePsu::modeSet(){
    if(mode_set == 0){
        mode_set = 1;
        increment = 1.0;
        set_value = &set_v;
    }else if(mode_set == 1){
        mode_set = 2; 
        increment = 1.0;  
        set_value = &set_a;          
    }else{
        mode_set = 0;
        increment = 0;
    }
}

void ModePsu::process(void){
    if(BUTTON_GetEvents() == BUTTON_PRESSED){
        if(mode_set){
            count = 0;
            switch(psu_state.input.value){
                case BUTTON_SET: count = BLINK_TIME_MASK; break;
                case BUTTON_UP: *set_value += increment; break;
                case BUTTON_DOWN: *set_value -= increment; break;
                case BUTTON_LEFT: increment *= 10.0; break;
                case BUTTON_RIGHT: increment /= 10.0; break;
            }       
        }
    }

    if(mode_set){
        if((++count) & BLINK_TIME_MASK){
             if(mode_set == 1){
                LCD_Fill(VOLTAGE_DRO_POS, 64, 20, BLACK);
                TEXT_dro(CURRENT_DRO_POS, set_a, 2);
            }else{
                LCD_Fill(CURRENT_DRO_POS, 64, 20, BLACK);
                TEXT_dro(VOLTAGE_DRO_POS, set_v, 1);
            }

        }else{
            TEXT_dro(VOLTAGE_DRO_POS, set_v, 1);
            TEXT_dro(CURRENT_DRO_POS, set_a, 2);
        }

       
    }else{
        TEXT_dro(VOLTAGE_DRO_POS, set_v, 1);
        TEXT_dro(CURRENT_DRO_POS, set_a, 2);
    }
}
void ModeLoad::redraw(void){
    LCD_Fill(0, 0, LCD_W, LCD_H, BLACK);
    TEXT_drawGfx(LOAD_ICON_POS, (uint8_t*)&icon_load[0]);
}

void setMode(uint8_t mode){
    
    if(mode >= MAX_SCREENS){
        return;
    }

    psu_state.mode_select = mode;
    LCD_Fill(MODE_ICONS_AREA_POS_S, MODE_ICONS_AREA_SIZE, BLACK);
    psu_state.screen = screens[mode];
    (psu_state.screen)->redraw();
}

void cycleMode(void){
uint8_t mode = psu_state.mode_select + 1;
    
    if(mode == MAX_SCREENS ){
        mode = 0;
    }
    
    setMode(mode);
}

void setOutput(uint8_t en){
    psu_state.output_en = en;
    if(psu_state.output_en){
        TEXT_drawGfx(OUTPUT_ICON_POS, (uint8_t*)&icon_out[0]);
    }else{
        LCD_Fill(OUTPUT_ICON_POS, icon_out[0], icon_out[1], BLACK);
    }
}

void toggleOutput(void){
    setOutput(!psu_state.output_en);
}

void checkButtons(){
    BUTTON_Read();
    if(BUTTON_GetEvents() == BUTTON_PRESSED){
        psu_state.input.last_value = psu_state.input.value;
        psu_state.input.value = BUTTON_GetValue();

        switch(psu_state.input.value){
            case BUTTON_MODE: cycleMode(); break;
            case BUTTON_OUT: toggleOutput(); break;
            case BUTTON_SET: psu_state.screen->modeSet(); break;
        }
    }
}

/**
 * Called every 10ms by Timer4 interrupt, as console
 * may block the main thread, having a secondary loop
 * ensures operation of lcd update and button handling
 * */
void tskPsu(void *ptr){
 static TickType_t xLastWakeTime;
    TEXT_Init();    

    setMode(0);

    while(1){
        checkButtons();
        HAL_GPIO_TogglePin(GPIOA, DBG_Pin);
        (psu_state.screen)->process();
        HAL_GPIO_TogglePin(GPIOA, DBG_Pin);
        LCD_Update();
        vTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS(UPDATE_INTERVAL));
    }
}

void tskConsole(void *ptr){
VoltageDro vdro;
Console console;
ConsoleHelp help;
CmdAdc adc1;
CmdPwm pwm;
CmdDfu dfu;
CmdPwr pwr;
CmdOut out;
CmdIo io;
    
    vcom.init();    
    console.init(&vcom, "PSU >");
    console.addCommand(&help);

    console.addCommand(&vdro);
    console.addCommand(&adc1);
    console.addCommand(&pwm);
    console.addCommand(&dfu);
    console.addCommand(&pwr); 
    console.addCommand(&out); 
    console.addCommand(&io); 

    while(1){
        console.process();
    }
}

extern "C" void psu(void){  

    //PWM_Init((uint16_t*)pwm_start_values);

    //ADC_Init(ADC_INTERVAL);
    //ADC_SetCallBack(UpdateResult);

    //setInterval(psu_v3_loop,APP_INTERVAL);

    xTaskCreate( tskConsole, "CLI", configMINIMAL_STACK_SIZE * 4, NULL, 3, NULL );
    xTaskCreate( tskPsu, "PSU", configMINIMAL_STACK_SIZE * 4, NULL, 3, NULL );
}


