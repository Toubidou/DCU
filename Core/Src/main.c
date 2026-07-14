/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "can.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#include <stdio.h>
#include <string.h>

CAN_TxHeaderTypeDef tx_header;
CAN_RxHeaderTypeDef rx_header;
uint32_t tx_mailbox;
uint8_t rx_data[16];
uint8_t flag_recv_data = 0;
/* ---------- 简易串口打印 ---------- */
char uart_buf[128];

void uart_print(const char *s)
{
	HAL_UART_Transmit(&huart2, (uint8_t *)s, strlen(s), 100);
}

void uart_print_hex(const char *label, uint32_t val)
{
	sprintf(uart_buf, "%s = 0x%08lX\r\n", label, val);
	uart_print(uart_buf);
}

/* ---------- 寄存器 dump ---------- */
void dump_all_regs(void)
{
	uint32_t can_mcr, can_msr, can_esr, can_btr, can_fmr, can_tsr;

	can_mcr = READ_REG(CAN1->MCR);
	can_msr = READ_REG(CAN1->MSR);
	can_esr = READ_REG(CAN1->ESR);
	can_btr = READ_REG(CAN1->BTR);
	can_fmr = READ_REG(CAN1->FMR);
	can_tsr = READ_REG(CAN1->TSR);

	uart_print("\r\n--- CAN1 ---\r\n");
	uart_print_hex("  MCR", can_mcr);
	uart_print_hex("  MSR", can_msr);
	uart_print_hex("  ESR", can_esr);
	uart_print_hex("  BTR", can_btr);
	uart_print_hex("  FMR", can_fmr);
	uart_print_hex("  TSR", can_tsr);
	uart_print("--- end ---\r\n");
}

/* ---------- CAN 初始化 ---------- */
void bsp_can_create(void)
{
	CAN_FilterTypeDef sFilterConfig;

	uart_print(">>> bsp_can_create() start\r\n");
	dump_all_regs();

	/* 1. 配置滤波器 */
	sFilterConfig.FilterBank = 0;
	sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
	sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
	sFilterConfig.FilterIdHigh = 0x0000;
	sFilterConfig.FilterIdLow = 0x0000;
	sFilterConfig.FilterMaskIdHigh = 0x0000;
	sFilterConfig.FilterMaskIdLow = 0x0000;
	sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
	sFilterConfig.FilterActivation = ENABLE;

	if (HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig) != HAL_OK)
	{
		uart_print("  FAIL: ConfigFilter\r\n");
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
			uart_print("  TIMEOUT! INAK still set.\r\n");
			dump_all_regs();
			return;
		}
	}

	uart_print("  INAK cleared, CAN started.\r\n");

	/* 3. 发送帧头 */
	tx_header.StdId = 0x100;
	tx_header.ExtId = 0;
	tx_header.IDE = CAN_ID_STD;
	tx_header.RTR = CAN_RTR_DATA;
	tx_header.DLC = 8;
	tx_header.TransmitGlobalTime = DISABLE;

	uart_print(">>> bsp_can_create() SUCCESS\r\n\r\n");
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *CanHandle)
{
	/* Get RX message */
	if (HAL_CAN_GetRxMessage(CanHandle, CAN_RX_FIFO0, &rx_header, rx_data) != HAL_OK)
	{
		/* Reception Error */
		Error_Handler();
	}
	flag_recv_data = 1;
}
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{

	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_CAN1_Init();
	MX_TIM2_Init();
	MX_USART2_UART_Init();
	/* USER CODE BEGIN 2 */
	bsp_can_create();
	uint8_t can_test_data[8] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37};
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	// uint32_t loop_cnt = 0;
	while (1)
	{
		// uint32_t msr = READ_REG(CAN1->MSR);
		// uint32_t esr = READ_REG(CAN1->ESR);
		// uint32_t tec = (esr & CAN_ESR_TEC) >> CAN_ESR_TEC_Pos;
		// uint32_t rec = (esr & CAN_ESR_REC) >> CAN_ESR_REC_Pos;

		// /* 检查是否意外回到 init 模式 */
		// if ((msr & CAN_MSR_INAK) != 0U) {
		// 	sprintf(uart_buf, "[%lu] INAK=1! ESR=0x%08lX\r\n", loop_cnt, esr);
		// 	uart_print(uart_buf);
		// 	HAL_Delay(1000);
		// 	loop_cnt++;
		// 	continue;
		// }

		/* 发送 CAN 帧 */
		HAL_StatusTypeDef ret = HAL_CAN_AddTxMessage(&hcan1, &tx_header, can_test_data, &tx_mailbox);

		/* 发完后重新读取 ESR (TEC 可能已变化) */
		// esr = READ_REG(CAN1->ESR);
		// tec = (esr & CAN_ESR_TEC) >> CAN_ESR_TEC_Pos;
		// rec = (esr & CAN_ESR_REC) >> CAN_ESR_REC_Pos;

		// sprintf(uart_buf, "[%lu] TX:ret=%d TEC=%lu REC=%lu ESR=0x%08lX\r\n",
		//         loop_cnt, ret, tec, rec, esr);
		// uart_print(uart_buf);

		// HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_0);
		HAL_Delay(1000);
		// loop_cnt++;

		if (flag_recv_data == 1)
		{
			HAL_UART_Transmit(&huart2, rx_data, 16, 100);
			flag_recv_data = 0;
		}

		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	/** Configure the main internal regulator output voltage
	 */
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 4;
	RCC_OscInitStruct.PLL.PLLN = 168;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 4;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
	{
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
	{
	}
	/* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
	   ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
