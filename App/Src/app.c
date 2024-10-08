#include "app.h"
#include "main.h"
#include "stdlib.h"
#include "string.h"

// 64 length
// 512 bits long

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;

// bits 0, 1, 2 are rgb
static uint8_t matrix[32][64] = {0};

static uint8_t idx = 0; // 0-64
static uint8_t scan = 0; // 0-15

static uint8_t clk = 0; // 0-1

static uint8_t dim = 0;

static inline void set_scan(uint8_t num) {
    if (num & 1) {
        GPIOB->BSRR = 1<<10;
    }else{
        GPIOB->BSRR = 1<<(10+16);
    }
    if (num & 2) {
        GPIOB->BSRR = 1<<15;
    }else{
        GPIOB->BSRR = 1<<(15+16);
    }
    if (num & 4) {
        GPIOA->BSRR = 1<<8;
    }else{
        GPIOA->BSRR = 1<<(8+16);
    }
    if (num & 8) {
        GPIOB->BSRR = 1<<1;
    }else{
        GPIOB->BSRR = 1<<(1+16);
    }
}

void App_Init(void) {
    HAL_GPIO_WritePin(OE_GPIO_Port, OE_Pin, 0);
    HAL_GPIO_WritePin(LAT_GPIO_Port, LAT_Pin, 0);

    HAL_TIM_Base_Start_IT(&htim1);
    HAL_TIM_Base_Start_IT(&htim2);
}

uint16_t delay = 0;
const uint8_t res = 7;
uint8_t count = 0;
uint8_t red = 0;

void App_Update(void) {

}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim) {
    if (htim == &htim2) {
        if (!delay) {
            red = ~(0xFF << count);
            count++;
            if (count > res) {
                count = 0;
            }
        }
        return;
    }
    if (clk) {
        GPIOA->BSRR = 1<<9;//clk
        GPIOB->BSRR = 1<<(2+16);//lat
        GPIOC->BSRR = 1<<(7+16);//oe
    }else {
        GPIOA->BSRR = 1<<(9+16);
        if (!idx) {
            GPIOB->BSRR = 1<<2;//lat
            GPIOC->BSRR = 1<<7;//oe
            set_scan(scan++);
            scan &= 0x0F;
            if (!scan) {
                dim++;
                if (dim > res) {
                    dim = 0;
                }
            }
        }

        if (red>>dim) {
            GPIOA->BSRR = 1<<(10);
            GPIOB->BSRR = 1<<(13);

            GPIOC->BSRR = 1<<(4);
            GPIOB->BSRR = 1<<(4);

            GPIOB->BSRR = 1<<(5);
            GPIOB->BSRR = 1<<(14);
        }else{
            GPIOA->BSRR = 1<<(10+16);
            GPIOB->BSRR = 1<<(13+16);

            GPIOC->BSRR = 1<<(4+16);
            GPIOB->BSRR = 1<<(4+16);

            GPIOB->BSRR = 1<<(5+16);
            GPIOB->BSRR = 1<<(14+16);
        }

        // HAL_GPIO_WritePin(R1_GPIO_Port, R1_Pin, (red>>dim)&1);
        // HAL_GPIO_WritePin(R2_GPIO_Port, R2_Pin, (red>>dim)&1);

        // HAL_GPIO_WritePin(R1_GPIO_Port, R1_Pin, matrix[scan][idx] & (1<<shift));
        // HAL_GPIO_WritePin(G1_GPIO_Port, G1_Pin, matrix[scan][idx] & (2<<shift));
        // HAL_GPIO_WritePin(B1_GPIO_Port, B1_Pin, matrix[scan][idx] & (4<<shift));

        // HAL_GPIO_WritePin(R2_GPIO_Port, R2_Pin, matrix[scan | 0x10][idx] & (1<<shift));
        // HAL_GPIO_WritePin(G2_GPIO_Port, G2_Pin, matrix[scan | 0x10][idx] & (2<<shift));
        // HAL_GPIO_WritePin(B2_GPIO_Port, B2_Pin, matrix[scan | 0x10][idx] & (4<<shift));

        idx++;
        idx &= 0x3F;
    }
    clk = !clk;
}

