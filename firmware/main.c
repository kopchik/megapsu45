#include "ch.h"
#include "hal.h"
#define SPI_CLOCK_DIV256 (SPI_CR1_BR_2 | SPI_CR1_BR_1 | SPI_CR1_BR_0)

/*
 * Red LED blinker thread, times are in milliseconds.
 */
static THD_WORKING_AREA(waThread1, 128);
static THD_FUNCTION(Thread1, arg) {

  (void)arg;
  chRegSetThreadName("blinker");
  while (true) {
    palClearPad(GPIOA, GPIOA_LED_GREEN);
    chThdSleepMilliseconds(500);
    palSetPad(GPIOA, GPIOA_LED_GREEN);
    chThdSleepMilliseconds(500);
  }
}

//TODO: set NSS
static const SPIConfig spicfg = { NULL, GPIOB, 12, SPI_CLOCK_DIV256 };


int main(void) {
  halInit();
  chSysInit();
  spiInit();

  spiStart(&SPID1, &spicfg);
  char test[] = "test";
  spiSend(&SPID1, sizeof(test), test);

  sdStart(&SD2, NULL);

  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);

  while (true) {
    if (!palReadPad(GPIOC, GPIOC_BUTTON))
    {}
    chThdSleepMilliseconds(500);
  }
}
