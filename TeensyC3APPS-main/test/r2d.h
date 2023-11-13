#ifndef _R2D_H_
#define _R2D_H_

#define POWER_ON_DELAY_MS 15000

typedef enum _r2d_mode {
    R2D_MODE_STARTUP,
    R2D_MODE_IDLE,
    R2D_MODE_DRIVE,
    R2D_MODE_ERROR
} r2d_mode;

// int check_bamocar();
// void send_to_bamocar(int value_bamo);
// int check_BMS();
void play_r2d_sound();
r2d_mode r2d_state_machine(r2d_mode cur_state, int apps_value);

#endif  // _R2D_H_