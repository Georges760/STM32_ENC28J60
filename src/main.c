/**
 ******************************************************************************
 * File Name          : main.c
 * Description        : Main program body
 ******************************************************************************
 *
 * COPYRIGHT(c) 2016 GPTechinno Limited
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of GPTechinno Limited nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */

/* Includes */
#include "stm32f4xx_hal.h"
#include "Ethernet.h"
#include "string.h"

/* Private macro */
#define MYWWWPORT 80
#define BUFFER_SIZE 550

/* Private variables */
SPI_HandleTypeDef SpiHandle;

static uint8_t mymac[6] = { 0x54, 0x57, 0x51, 0x10, 0x00, 0x25 };
//static uint8_t myip[4] = { 192, 168, 0, 15};
static uint8_t buf[BUFFER_SIZE + 1];
uint16_t plen, dat_p;

/* For DHCP */
static uint8_t myip[4] = { 0, 0, 0, 0 };
static uint8_t mynetmask[4] = { 0, 0, 0, 0 };

// Default gateway. The ip address of your DSL router. It can be set to the same as
// websrvip the case where there is no default GW to access the
// web server (=web server is on the same lan as this host)
static uint8_t gwip[4] = { 192, 168, 0, 1 };

static uint8_t dnsip[4] = { 0, 0, 0, 0 };
static uint8_t dhcpsvrip[4] = { 0, 0, 0, 0 };

/* Private function prototypes */
uint16_t http200ok(void);
uint16_t print_webpage(uint8_t *buf);
uint16_t print_webpage2(uint8_t *buf);
void SystemClock_Config(void);
static void GPIO_Init(void);
static void SPI_Init(void);

/* Private functions */
int main(void) {
	/* MCU Configuration */
	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* Configure the system clock */
	SystemClock_Config();

	/* Initialize all configured peripherals */
	GPIO_Init();
	ETH_RST_OFF();
	ETH_NSS_RELEASE();

	SPI_Init();

	/* Initialize Ethernet */
	ETH_RST_ON();
	HAL_Delay(5);
	Ethernet_Init(mymac);

	if (Ethernet_Revision() <= 0) {
		// Failed to access ENC28J60
		while (1);    // Just loop here
	}

	// Get IP Address details
	if (Ethernet_allocate_ip_address(buf, BUFFER_SIZE, mymac, 80, myip, mynetmask, gwip, dhcpsvrip, dnsip) > 0) {
		// Display the results:
	} else {
		// Failed to get IP address"
		while (1);
	}

	while (1) {
		// read packet, handle ping and wait for a tcp packet:
		dat_p = Ethernet_packetloop_icmp_tcp(buf, Ethernet_PacketReceive(BUFFER_SIZE, buf));

		if (dat_p != 0) {
			if (strncmp("GET ", (char *) &(buf[dat_p]), 4) != 0) {
				dat_p = http200ok();
				dat_p = Ethernet_fill_tcp_data(buf, dat_p, "<h1>200 OK</h1>");
			} else if (strncmp("/ ", (char *) &(buf[dat_p + 4]), 2) == 0) {
				dat_p = print_webpage(buf);
			} else if (strncmp("/test ", (char *) &(buf[dat_p + 4]), 2) == 0) {
				dat_p = print_webpage2(buf);
			} else {
				dat_p =	Ethernet_fill_tcp_data(buf, 0, "HTTP/1.0 401 Unauthorized\r\nContent-Type: text/html\r\n\r\n<h1>401 Unauthorized</h1>");
			}
			Ethernet_www_server_reply(buf, dat_p);
		}
	}
}

uint16_t http200ok(void) {
	return (Ethernet_fill_tcp_data(buf, 0, "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\nPragma: no-cache\r\n\r\n"));
}

uint16_t print_webpage(uint8_t *buf) {
	uint16_t plen;
	plen = http200ok();
	plen = Ethernet_fill_tcp_data(buf, plen, "<html><head><title>STM32F4 ENC28J60 Ethernet Demo</title></head><body>");
	plen = Ethernet_fill_tcp_data(buf, plen, "<center><h1>Welcome to STM32F4 ENC28J60 Ethernet Demo</h1>");
	plen = Ethernet_fill_tcp_data(buf, plen, "<br> Control digital outputs (not implemented yet)");
	plen = Ethernet_fill_tcp_data(buf, plen, "<br> Read digital analog inputs (not implemented yet)");
	plen = Ethernet_fill_tcp_data(buf, plen, "<br></font></h2>");
	plen = Ethernet_fill_tcp_data(buf, plen, "</center><hr>");
	plen = Ethernet_fill_tcp_data(buf, plen, "</body></html>");
	return (plen);
}

uint16_t print_webpage2(uint8_t *buf) {
	uint16_t plen;
	plen = http200ok();
	plen = Ethernet_fill_tcp_data(buf, plen, "<html><head><title>Test page</title></head><body>");
	plen = Ethernet_fill_tcp_data(buf, plen, "<center><h1>Second page</h1>");
	plen = Ethernet_fill_tcp_data(buf, plen, "<br>Hello World");
	plen = Ethernet_fill_tcp_data(buf, plen, "<br></font></h2>");
	plen = Ethernet_fill_tcp_data(buf, plen, "</center><hr>");
	plen = Ethernet_fill_tcp_data(buf, plen, "</body></html>");
	return (plen);
}

void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_ClkInitTypeDef RCC_ClkInitStruct;

	__PWR_CLK_ENABLE();

	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = 16;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = 16;
	RCC_OscInitStruct.PLL.PLLN = 336;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
	RCC_OscInitStruct.PLL.PLLQ = 2;
	RCC_OscInitStruct.PLL.PLLR = 2;
	HAL_RCC_OscConfig(&RCC_OscInitStruct);

	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
	HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);

	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);

	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	/* SysTick_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* GPIO init function */
void GPIO_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct;

	/* GPIO Ports Clock Enable */
	__GPIOC_CLK_ENABLE(); // why ?
	__GPIOH_CLK_ENABLE(); // why ?

	/* Configure GPIO pin : ETH_NSS */
	ETH_NSS_GPIO_CLK_ENABLE();
	GPIO_InitStruct.Pin = ETH_NSS_GPIO_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(ETH_NSS_GPIO_PORT, &GPIO_InitStruct);

	/* Configure GPIO pin : ETH_RST */
	ETH_RST_GPIO_CLK_ENABLE();
	GPIO_InitStruct.Pin = ETH_RST_GPIO_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(ETH_RST_GPIO_PORT, &GPIO_InitStruct);
}

/* SPI init function */
void SPI_Init(void) {

	SpiHandle.Instance = SPIx;
	SpiHandle.Init.Mode = SPI_MODE_MASTER;
	SpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
	SpiHandle.Init.Direction = SPI_DIRECTION_2LINES;
	SpiHandle.Init.CLKPhase = SPI_PHASE_1EDGE;
	SpiHandle.Init.CLKPolarity = SPI_POLARITY_LOW;
	SpiHandle.Init.DataSize = SPI_DATASIZE_8BIT;
	SpiHandle.Init.FirstBit = SPI_FIRSTBIT_MSB;
	SpiHandle.Init.TIMode = SPI_TIMODE_DISABLED;
	SpiHandle.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLED;
	SpiHandle.Init.CRCPolynomial = 10;
	SpiHandle.Init.NSS = SPI_NSS_SOFT;
	HAL_SPI_Init(&SpiHandle);
}

void ENC28J60_DelayUs(uint32_t delay) {
	do {
		asm volatile (	"MOV R0,%[loops]\n\t"\
				"1: \n\t"\
				"SUB R0, #1\n\t"\
				"CMP R0, #0\n\t"\
				"BNE 1b \n\t" : : [loops] "r" (20*delay) : "memory"\
			      ); // test logic analyzer, target 100us : 20->97us 21->127us
	} while(0);
}

void ENC28J60_DelayMs(uint32_t delay) {
	HAL_Delay(delay);
}

uint32_t ENC28J60_GetMs(void) {
	return HAL_GetTick();
}

void ENC28J60_EnableChip(void) {
	ETH_NSS_SELECT();
}

void ENC28J60_DisableChip(void) {
	ETH_NSS_RELEASE();
}

uint8_t ENC28J60_TransceiveByte(uint8_t data) {
	uint8_t received;
	if (HAL_SPI_TransmitReceive(&SpiHandle, &data, &received, 1, 1000) == HAL_OK) {
		return received;
	}
	return 0;
}


#ifdef USE_FULL_ASSERT

/**
 * @brief Reports the name of the source file and the source line number
 * where the assert_param error has occurred.
 * @param file: pointer to the source file name
 * @param line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line) {
	/* User can add his own implementation to report the file name and line number,
	 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
}

#endif

/**
 * @}
 */

/**
 * @}
 */

/************************ (C) COPYRIGHT GPTechinno Limited *****END OF FILE****/
