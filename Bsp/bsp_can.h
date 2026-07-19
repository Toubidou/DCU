#ifndef _BSP_CAN_H_
#define _BSP_CAN_H_

#include <stdio.h>
#include <string.h>
#include "can.h"

void bsp_can_create(void);
void bsp_can_send(uint8_t *data, uint8_t len);
void bsp_can_receive(uint8_t *data, uint8_t *len);

#endif // _BSP_CAN_H_
