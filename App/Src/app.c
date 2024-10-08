#include "app.h"
#include "main.h"
#include "stdlib.h"
#include "string.h"

// 64 length
// 512 bits long

extern TIM_HandleTypeDef htim1;

// bits 0, 1, 2 are rgb
static uint8_t matrix[32][64] = {0};

static uint8_t idx = 0; // 0-64
static uint8_t scan = 0; // 0-15

static uint8_t clk = 0; // 0-1

static uint8_t dim = 0;

static void set_scan(uint8_t num) {
    HAL_GPIO_WritePin(A_GPIO_Port, A_Pin, num & 1);
    HAL_GPIO_WritePin(B_GPIO_Port, B_Pin, num & 2);
    HAL_GPIO_WritePin(C_GPIO_Port, C_Pin, num & 4);
    HAL_GPIO_WritePin(D_GPIO_Port, D_Pin, num & 8);
}

void App_Init(void) {
    HAL_GPIO_WritePin(OE_GPIO_Port, OE_Pin, 0);
    HAL_GPIO_WritePin(LAT_GPIO_Port, LAT_Pin, 0);

    HAL_TIM_Base_Start_IT(&htim1);
    srand(0);
}

const uint8_t res = 3;
uint8_t count = 0;
uint8_t red = 0;
void App_Update(void) {
    red = ~(0xFF << count);
    count++;
    if (count > res) {
        count = 0;
    }
    
    HAL_Delay(1000);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim) {
    UNUSED(htim);
    HAL_GPIO_WritePin(CLK_GPIO_Port, CLK_Pin, clk);
    if (clk) {
        HAL_GPIO_WritePin(LAT_GPIO_Port, LAT_Pin, 0);
        HAL_GPIO_WritePin(OE_GPIO_Port, OE_Pin, 0);
    }else {
        if (!idx) {
            HAL_GPIO_WritePin(LAT_GPIO_Port, LAT_Pin, 1);
            HAL_GPIO_WritePin(OE_GPIO_Port, OE_Pin, 1);
            set_scan(scan++);
            scan &= 0x0F;
            if (!scan) {
                dim++;
                if (dim > res) {
                    dim = 0;
                }
            }
        }

        HAL_GPIO_WritePin(R1_GPIO_Port, R1_Pin, red>>dim);
        HAL_GPIO_WritePin(R2_GPIO_Port, R2_Pin, red>>dim);

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

