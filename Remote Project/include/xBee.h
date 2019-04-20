// xBee.h

#ifndef __XBEE_H__
#define __XBEE_H__ 1

void xBee_Init(void);

uint8_t xBee_InByte(void);

void xBee_OutByte(uint8_t byte);

uint32_t xBee_InMessage(void);

void xBee_OutMessage(uint32_t msg);

void xBee_Packager(void);

#endif // __XBEE_H__
