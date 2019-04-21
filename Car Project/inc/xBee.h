// xBee.h

#ifndef __XBEE_H__
#define __XBEE_H__ 1

void xBee_Init(void);

uint8_t xBee_InByte(void);

void xBee_OutByte(uint8_t byte);

void xBee_InCmd(uint8_t* left, uint8_t* right);

#endif // __XBEE_H__
