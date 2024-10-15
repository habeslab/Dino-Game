#include "joystick.h"


void joystick_init(ADC_HandleTypeDef *hadc1, uint32_t *VR) {
    // Inizializza il joystick (se necessario)
    // La configurazione dell'ADC e del DMA dovrebbe essere già fatta nel main.c
    HAL_ADC_Start_DMA(hadc1, VR, 2);
}


int joystick_up(uint32_t *VR) {
    // Leggi il valore del joystick dall'asse Y
    int joy_Y = VR[1];
    //serial_print("Sono qui nel joystick-up() , value : %d\n",joy_Y);

    // Soglia per rilevare il movimento verso l'alto (da calibrare)
    int threshold_up = 500;

    // Ritorna 1 se il joystick è spostato verso l'alto, altrimenti 0
    return (joy_Y < threshold_up) ? 1 : 0;
}


int joystick_down(uint32_t *VR) {
    // Leggi il valore del joystick dall'asse Y
    int joy_Y = VR[1];
    //serial_print("Sono qui nel joystick-down() , value : %d\n",joy_Y);

    // Soglia per rilevare il movimento verso il basso (da calibrare)
    return (joy_Y > 3200) ? 1 : 0;
}

int joystick_stable(uint32_t *VR) {
    // Leggi il valore del joystick dall'asse Y
    int joy_Y = VR[1];
    //serial_print("Sono qui nel joystick-down() , value : %d\n",joy_Y);

    return (joy_Y>=3000 && joy_Y<=3200) ? 1: 0;
}

int joystick_left(uint32_t *VR) {
    // Leggi il valore del joystick dall'asse Y
    int joy_X = VR[0];
    //serial_print("Sono qui nel joystick-down() , value : %d\n",joy_Y);

    return (joy_X>3200) ? 1: 0;
}

int joystick_right(uint32_t *VR) {
    // Leggi il valore del joystick dall'asse Y
    int joy_X = VR[0];
    //serial_print("Sono qui nel joystick-up() , value : %d\n",joy_Y);

    // Soglia per rilevare il movimento verso l'alto (da calibrare)
    int threshold_up = 500;

    // Ritorna 1 se il joystick è spostato verso l'alto, altrimenti 0
    return (joy_X < threshold_up) ? 1 : 0;
}
