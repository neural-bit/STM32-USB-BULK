#include "main.h"
#include "cmsis_os.h"
#include "usb_device.h"
#include <math.h>

osThreadId streamTaskHandle;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
void StartStreamTask(void const * argument);
extern int is_streaming;
extern USBD_HandleTypeDef hUsbDeviceFS;


#include <math.h>

#define PI 3.14159265
#define CHANNELS 8
#define SAMPLE_SIZE_BYTES (CHANNELS * 4) // 4 bytes per channel (int32_t)
#define TX_BUFFER_SIZE SAMPLE_SIZE_BYTES
#define SAMPLING_FREQ_HZ 1200
#define DELAY_US (1000000 / SAMPLING_FREQ_HZ)

float phase = 0.0f;
uint16_t sample_index = 0;
const float freq = 10.0f;  // 10 Hz waveform
const float phase_increment = 2 * PI * freq / SAMPLING_FREQ_HZ;

void generateSignal(uint8_t *tx_buffer)
{
    static float phase = 0.0f;
    static uint16_t sample_index = 0;

    const float freq = 10.0f;
    const float phase_increment = 2 * PI * freq / SAMPLING_FREQ_HZ;

    // 1. Sine Wave
    int32_t ch1 = (int32_t)(10 * sinf(phase));

    // 2. Cosine Wave
    int32_t ch2 = (int32_t)(10 * cosf(phase));

    // 3. Sawtooth Wave (range -10 to +10)
    float saw = fmodf(phase, 2 * PI) / (2 * PI);  // 0.0 to 1.0
    int32_t ch3 = (int32_t)(20 * saw - 10);       // -10 to 10

    // 4. Triangle Wave (range -10 to +10)
    float tri = 2.0f * fabsf(2.0f * saw - 1.0f);  // 0 to 2 -> 0 to 1 to 0
    int32_t ch4 = (int32_t)(20 * (tri - 0.5f));   // -10 to 10

    // 5. Square Wave
    int32_t ch5 = (sinf(phase) > 0) ? 10 : -10;

    // 6. White Noise
    int32_t ch6 = (rand() % 21) - 10;  // random value from -10 to 10

    // 7. Step Function (every 100 samples)
    int32_t ch7 = (sample_index % 200 < 100) ? 10 : -10;

    // 8. Exponential Decay (simulate decay curve)
    float decay = expf(-phase);
    int32_t ch8 = (int32_t)(10 * decay);

    int32_t values[CHANNELS] = {
        ch1, ch2, ch3, ch4, ch5, ch6, ch7, ch8
    };

    // Pack into tx_buffer
    uint8_t *ptr = tx_buffer;
    for (int ch = 0; ch < CHANNELS; ++ch)
    {
        int32_t val = values[ch];
        *ptr++ = val & 0xFF;
        *ptr++ = (val >> 8) & 0xFF;
        *ptr++ = (val >> 16) & 0xFF;
        *ptr++ = (val >> 24) & 0xFF;
    }

    // Update state
    phase += phase_increment;
    if (phase > 2 * PI) phase -= 2 * PI;
    sample_index++;
}




int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();

  osThreadDef(streamTask, StartStreamTask, osPriorityHigh, 0, 128);
  streamTaskHandle = osThreadCreate(osThread(streamTask), NULL);

  osKernelStart();

  while (1)
  {

  }
}


void StartStreamTask(void const * argument)
{
    uint8_t tx_buffer[TX_BUFFER_SIZE];

    MX_USB_DEVICE_Init();

    while (hUsbDeviceFS.dev_state != USBD_STATE_CONFIGURED)
    {
        osDelay(100);
    }

    while (1)
    {
        if (is_streaming)
        {
            generateSignal(tx_buffer);

            if (USBD_CUSTOM_BULK_SendData(&hUsbDeviceFS, tx_buffer, TX_BUFFER_SIZE) == USBD_OK)
            {
                osDelay((DELAY_US + 999) / 1000);  // Ensure delay in ms
            }
        }
        else
        {
            osDelay(10);
        }
    }
}



/*void StartStreamTask(void const * argument)
{
  uint8_t tx_buffer[64];
  for (uint8_t i = 0; i < 64; i++)
  {
    tx_buffer[i] = i + 1;
  }

  while (1)
  {
    if (is_streaming)  // stream only when flag is set
    {
      if (USBD_CUSTOM_BULK_SendData(&hUsbDeviceFS, tx_buffer, 64) == USBD_OK)
      {
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);  // Toggle LED
      }
    }

    osDelay(500);
  }
}*/



void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}



static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}


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


void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
