
#include "psu.h"


/**
 * HW modules configuration
 * TIM4   program loop tick
 * TIM2   ADC Trigger
 * DMA1   ADC data transfer
 * ADC1-2 Dual configuration for simultaneous convertion
 * TIM3   PWM signals
 * */

static Psu cpsu;
static Load cload;
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
        	return CMD_BAD_PARAM;
        }

		if(pdata == 1){
			setOutput(1);
		}else{
			setOutput(0);
		}

        cycleMode();
		
		return CMD_OK;
	}	
};


void UpdateResult(uint16_t *adcres){
static double c = 99.1;
    //SEVEN_Double(1,1, (3.3 * adcres[2]) / 4096);
    //SEVEN_Double(65,1, (3.3 * adcres[3]) / 4096);
    TEXT_dro(VOLTAGE_DRO_POS, c,1);
    c += 0.1;
    if(c == 100.0) c = 0;
    LCD_Update();
}

void Psu::redraw(void){
    LCD_Fill(0, 0, LCD_W, LCD_H, BLACK);
    //TEXT_setFont(&defaultBoldFont);
    //TEXT_setFont(&lcdFont);
    TEXT_setFont(&pixelDustFont);
    //TEXT_setFont(&defaultFont);
    TEXT_print(0,0, "88.8W");
    TEXT_setFont(&font_seven_seg);
    TEXT_dro(VOLTAGE_DRO_POS, 88.8, 1);
    TEXT_dro(CURRENT_DRO_POS, 8.88, 2);
    TEXT_drawGfx(PSU_ICON_POS, (uint8_t*)&icon_psu[0]);
}

void Psu::process(void){
    static double c = 0.1;
    TEXT_dro(VOLTAGE_DRO_POS, c, 1);
    c += 0.1;
    if(c == 100.0) c = 0;  
}

void Load::redraw(void){
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
    setOutput(psu_state.output_en);
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

void checkButtons(Input *inp){
    BUTTON_Read();
    if(BUTTON_GetEvents() == BUTTON_PRESSED){
        inp->last_value = inp->value;
        inp->value = BUTTON_GetValue();

        if(inp->value & BUTTON_MODE){
            cycleMode();
        }

        if(inp->value & BUTTON_MODE){
            toggleOutput();
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
    
    vcom.init();    
    console.init(&vcom, "PSU >");
    console.addCommand(&help);

    console.addCommand(&vdro);
    console.addCommand(&adc1);
    console.addCommand(&pwm);
    console.addCommand(&dfu);
    console.addCommand(&pwr); 
    console.addCommand(&out); 

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


