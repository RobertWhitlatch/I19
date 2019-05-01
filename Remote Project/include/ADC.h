// ADC.h

#ifndef __ADC_H__
#define __ADC_H__ 1

void ADC_Init(void);

uint32_t medianFilter0(uint32_t input);

uint32_t medianFilter1(uint32_t input);

uint8_t fetchTableValue(uint32_t speed);

#endif // __ADC_H__
