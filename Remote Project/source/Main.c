// Main.c

#include "Master.h"

int main(void){
    PLL_Init(Bus80MHz);
    Output_Init();
    RemoteMain();
}
