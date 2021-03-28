// Peripheral usage
//#include "stm32f0xx_hal.h"
#include "stm32f1xx_hal.h"

extern TIM_HandleTypeDef htim2;
DMA_HandleTypeDef hdma_tim2_ch1;

#include "sk6812.h"

/*#define PWM_HI (7)
#define PWM_LO (3)*/
#define PWM_HI (42)
#define PWM_LO (17)

// LED parameters
#define NUM_BPP (3) // WS2812B
//#define NUM_BPP (4) // SK6812
#define NUM_PIXELS (8)
#define NUM_BYTES (NUM_BPP * NUM_PIXELS)
#define BITS_PER_COLOR_CHANNEL 8
// LED color buffer
uint8_t rgb_arr[NUM_BYTES] = {0};

// LED write buffer
#define WR_BUF_LEN (NUM_BPP * 8 * 2)
uint8_t wr_buf[WR_BUF_LEN] = {0};
uint_fast8_t buf_p = 0;

static inline uint8_t scale8(uint8_t x, uint8_t scale) {
  return ((uint16_t)x * scale) >> 8;
}

// Set a single color (RGB) to index
void led_set_RGB(uint8_t index, uint8_t r, uint8_t g, uint8_t b) {
  rgb_arr[3 * index] = g;// scale8(g, 0xB0); // g;
  rgb_arr[3 * index + 1] = r;
  rgb_arr[3 * index + 2] = b; //scale8(b, 0xF0); // b;
}

// Set a single color (RGBW) to index
void led_set_RGBW(uint8_t index, uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
  led_set_RGB(index, r, g, b);
}

// Set all colors to RGB
void led_set_all_RGB(uint8_t r, uint8_t g, uint8_t b) {
  for(uint_fast8_t i = 0; i < NUM_PIXELS; ++i) led_set_RGB(i, r, g, b);
}

// Set all colors to RGBW
void led_set_all_RGBW(uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
  for(uint_fast8_t i = 0; i < NUM_PIXELS; ++i) led_set_RGBW(i, r, g, b, w);
}

// Shuttle the data to the LEDs!
void led_render() {
  if(buf_p != 0 || hdma_tim2_ch1.State != HAL_DMA_STATE_READY) {
    // Ongoing transfer, cancel!
    for(uint8_t i = 0; i < WR_BUF_LEN; ++i) wr_buf[i] = 0;
    buf_p = 0;
    HAL_TIM_PWM_Stop_DMA(&htim2, TIM_CHANNEL_1);
    return;
  }
  // Ooh boi the first data buffer half (and the second!)
  for(uint_fast8_t i = 0; i < 8; ++i) {
    wr_buf[i     ] = ((rgb_arr[0] << i) & 0x80) == 0 ? PWM_LO : PWM_HI;
    wr_buf[i + 8 ] = ((rgb_arr[1] << i) & 0x80) == 0 ? PWM_LO : PWM_HI;
    wr_buf[i + 16] = ((rgb_arr[2] << i) & 0x80) == 0 ? PWM_LO : PWM_HI;
    wr_buf[i + 24] = ((rgb_arr[3] << i) & 0x80) == 0 ? PWM_LO : PWM_HI;
    wr_buf[i + 32] = ((rgb_arr[4] << i) & 0x80) == 0 ? PWM_LO : PWM_HI;
    wr_buf[i + 40] = ((rgb_arr[5] << i) & 0x80) == 0 ? PWM_LO : PWM_HI;
   // wr_buf[i +  8] = PWM_LO << (((rgb_arr[1] << i) & 0x80) > 0);
   // wr_buf[i + 16] = PWM_LO << (((rgb_arr[2] << i) & 0x80) > 0);
   // wr_buf[i + 24] = PWM_LO << (((rgb_arr[3] << i) & 0x80) > 0);
   // wr_buf[i + 32] = PWM_LO << (((rgb_arr[4] << i) & 0x80) > 0);
   // wr_buf[i + 40] = PWM_LO << (((rgb_arr[5] << i) & 0x80) > 0);
  }

  HAL_TIM_PWM_Start_DMA(&htim2, TIM_CHANNEL_1, (uint32_t *)wr_buf, WR_BUF_LEN);
  buf_p = 2; // Since we're ready for the next buffer
}

//void HAL_TIM_PWM_PulseFinishedHalfCpltCallback(TIM_HandleTypeDef *htim) {
void half_tx_update() {
  // DMA buffer set from LED(buf_p) to LED(buf_p + 1)
  if(buf_p < NUM_PIXELS) {
    // We're in. Fill the even buffer
    for(uint_fast8_t i = 0; i < 8; ++i) {
      wr_buf[i     ] = ((rgb_arr[3 * buf_p    ] << i) & 0x80) == 0 ? PWM_LO : PWM_HI;
      wr_buf[i + 8 ] = ((rgb_arr[3 * buf_p + 1] << i) & 0x80) == 0 ? PWM_LO : PWM_HI;
      wr_buf[i + 16] = ((rgb_arr[3 * buf_p + 2] << i) & 0x80) == 0 ? PWM_LO : PWM_HI;
      //wr_buf[i     ] = PWM_LO << (((rgb_arr[3 * buf_p    ] << i) & 0x80) > 0);
      //wr_buf[i +  8] = PWM_LO << (((rgb_arr[3 * buf_p + 1] << i) & 0x80) > 0);
      //wr_buf[i + 16] = PWM_LO << (((rgb_arr[3 * buf_p + 2] << i) & 0x80) > 0);
    }
    buf_p++;
  } else if (buf_p < NUM_PIXELS + 2) {
    // Last two transfers are resets. SK6812: 64 * 1.25 us = 80 us == good enough reset
  	//HAL_TIM_PWM_PulseFinishedCallback/                               WS2812B: 48 * 1.25 us = 60 us == good enough reset
    // First half reset zero fill
    for(uint8_t i = 0; i < WR_BUF_LEN / 2; ++i) wr_buf[i] = 0;
    buf_p++;
  }
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) {
  static uint_fast8_t channel_p=0;
  if ( buf_p < NUM_PIXELS) {
    TIM2->CCR1 = ((rgb_arr[buf_p] << channel_p) & 0x80) == 0 ? PWM_LO : PWM_HI ;
    channel_p++;
    if ( channel_p >= BITS_PER_COLOR_CHANNEL) {
      channel_p=0;
      buf_p++;
    }
  }
  else if (buf_p < NUM_PIXELS + 2) {   
    TIM2->CCR1 = 0; 
    channel_p++;
    if ( channel_p >= BITS_PER_COLOR_CHANNEL) {
      channel_p=0;
      buf_p++;
    }
  }
  else {
    buf_p=0;
  }
  return;
}
//void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) {
void full_tx_update() {
  // DMA buffer set from LED(buf_p) to LED(buf_p + 1)
  if(buf_p < NUM_PIXELS) {
    // We're in. Fill the odd buffer
    // WS2812B
    for(uint_fast8_t i = 0; i < 8; ++i) {
      wr_buf[i + 24] = ((rgb_arr[3 * buf_p    ] << i) & 0x80) == 0 ? PWM_LO : PWM_HI;
      wr_buf[i + 32] = ((rgb_arr[3 * buf_p + 1] << i) & 0x80) == 0 ? PWM_LO : PWM_HI;
      wr_buf[i + 40] = ((rgb_arr[3 * buf_p + 2] << i) & 0x80) == 0 ? PWM_LO : PWM_HI;
     // wr_buf[i + 24] = PWM_LO << (((rgb_arr[3 * buf_p    ] << i) & 0x80) > 0);
     // wr_buf[i + 32] = PWM_LO << (((rgb_arr[3 * buf_p + 1] << i) & 0x80) > 0);
     // wr_buf[i + 40] = PWM_LO << (((rgb_arr[3 * buf_p + 2] << i) & 0x80) > 0);
    }
    buf_p++;
  } else if (buf_p < NUM_PIXELS + 2) {
    // Second half reset zero fill
    for(uint8_t i = WR_BUF_LEN / 2; i < WR_BUF_LEN; ++i) wr_buf[i] = 0;
    ++buf_p;
  } else {
    // We're done. Lean back and until next time!
    buf_p = 0;
    HAL_TIM_PWM_Stop_DMA(&htim2, TIM_CHANNEL_1);
  }
}
