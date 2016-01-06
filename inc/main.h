/**
 ******************************************************************************
 * File Name          : main.h
 * Description        : This file contains the common defines of the application
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

/* Define to prevent recursive inclusion */
#ifndef __MAIN_H
#define __MAIN_H

/* Includes */

/* Private defines */

/************************* GPIO *************************/
#define ETH_NSS_GPIO_CLK_ENABLE()		__HAL_RCC_GPIOC_CLK_ENABLE()
#define ETH_NSS_GPIO_PORT				GPIOC
#define ETH_NSS_GPIO_PIN					GPIO_PIN_5
#define ETH_NSS_SELECT()					HAL_GPIO_WritePin(ETH_NSS_GPIO_PORT, ETH_NSS_GPIO_PIN, GPIO_PIN_RESET)
#define ETH_NSS_RELEASE()				HAL_GPIO_WritePin(ETH_NSS_GPIO_PORT, ETH_NSS_GPIO_PIN, GPIO_PIN_SET)

#define ETH_RST_GPIO_CLK_ENABLE()		__HAL_RCC_GPIOC_CLK_ENABLE()
#define ETH_RST_GPIO_PORT				GPIOC
#define ETH_RST_GPIO_PIN					GPIO_PIN_6
#define ETH_RST_OFF()					HAL_GPIO_WritePin(ETH_RST_GPIO_PORT, ETH_RST_GPIO_PIN, GPIO_PIN_RESET)
#define ETH_RST_ON()						HAL_GPIO_WritePin(ETH_RST_GPIO_PORT, ETH_RST_GPIO_PIN, GPIO_PIN_SET)

/************************* SPI *************************/
#define SPIx                             SPI2
#define SPIx_CLK_ENABLE()                __HAL_RCC_SPI2_CLK_ENABLE()

#define SPIx_FORCE_RESET()               __HAL_RCC_SPI2_FORCE_RESET()
#define SPIx_RELEASE_RESET()             __HAL_RCC_SPI2_RELEASE_RESET()

#define SPIx_SCK_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOB_CLK_ENABLE()
#define SPIx_SCK_GPIO_PIN                GPIO_PIN_10
#define SPIx_SCK_GPIO_PORT               GPIOB
#define SPIx_SCK_GPIO_AF                 GPIO_AF5_SPI2

#define SPIx_MOSI_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOC_CLK_ENABLE()
#define SPIx_MOSI_GPIO_PIN               GPIO_PIN_1
#define SPIx_MOSI_GPIO_PORT              GPIOC
#define SPIx_MOSI_GPIO_AF                GPIO_AF7_SPI2

#define SPIx_MISO_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOC_CLK_ENABLE()
#define SPIx_MISO_GPIO_PIN               GPIO_PIN_2
#define SPIx_MISO_GPIO_PORT              GPIOC
#define SPIx_MISO_GPIO_AF                GPIO_AF5_SPI2

/**
 * @}
 */

/**
 * @}
 */

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT GPTechinno Limited *****END OF FILE****/
