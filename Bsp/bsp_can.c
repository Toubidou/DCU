#include "bsp_can.h"
#include "app_log.h"

static CAN_TxHeaderTypeDef tx_header;
static CAN_RxHeaderTypeDef rx_header;
static uint32_t tx_mailbox;

uint8_t flag_recv_data = 0;
uint8_t rx_data[256];
/* ---------- CAN 初始化 ---------- */
void bsp_can_create(void)
{
	CAN_FilterTypeDef sFilterConfig;

	/* 1. 配置滤波器 */
	sFilterConfig.FilterBank = 0;
	sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
	sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
	sFilterConfig.FilterIdHigh = 0x0000;
	sFilterConfig.FilterIdLow = 0x0000;
	sFilterConfig.FilterMaskIdHigh = 0x0000;
	sFilterConfig.FilterMaskIdLow = 0x0000;
	sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO1;
	sFilterConfig.FilterActivation = ENABLE;

	if (HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig) != HAL_OK)
	{
		app_log("Failed to configure CAN filter\r\n");
		return;
	}

	/* 2. 手动退出初始化 */
	hcan1.State = HAL_CAN_STATE_LISTENING;
	CLEAR_BIT(CAN1->MCR, CAN_MCR_INRQ);

	uint32_t tickstart = HAL_GetTick();
	while ((CAN1->MSR & CAN_MSR_INAK) != 0U)
	{
		if ((HAL_GetTick() - tickstart) > 1000U)
		{
			app_log("  TIMEOUT! INAK still set.\r\n");
			return;
		}
	}

	/* 3. 使能 FIFO1 消息挂起中断 */
	HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO1_MSG_PENDING);

	/* 4. 发送帧头 */
	tx_header.StdId = 0x100;
	tx_header.ExtId = 0;
	tx_header.IDE = CAN_ID_STD;
	tx_header.RTR = CAN_RTR_DATA;
	tx_header.DLC = 8;
	tx_header.TransmitGlobalTime = DISABLE;
}
void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *CanHandle)
{
	/* Get RX message */
	if (HAL_CAN_GetRxMessage(CanHandle, CAN_RX_FIFO1, &rx_header, rx_data) != HAL_OK)
	{
		/* Reception Error */
		Error_Handler();
	}
	flag_recv_data = 1;
}

void bsp_can_send(uint8_t *data, uint8_t len)
{
    if (len > 8)
    {
        app_log("CAN data length exceeds 8 bytes\r\n");
        return;
    }

    HAL_StatusTypeDef ret = HAL_CAN_AddTxMessage(&hcan1, &tx_header, data, &tx_mailbox);
    if (ret != HAL_OK)
    {
        app_log("Failed to send CAN message\r\n");
    }
}
void bsp_can_receive(uint8_t *data, uint8_t *len)
{
    if (flag_recv_data)
    {
        memcpy(data, rx_data, 8);
        *len = rx_header.DLC;
        flag_recv_data = 0;
    }
    else
    {
        *len = 0; // No data received
    }
}