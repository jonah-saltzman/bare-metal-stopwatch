#include "../Inc/sys/stm32f439xx.h"

void render_display();

#define FIRST_DIGIT  (GPIO_ODR_OD5_Msk)
#define SECOND_DIGIT (GPIO_ODR_OD6_Msk)
#define THIRD_DIGIT  (GPIO_ODR_OD7_Msk)
#define FOURTH_DIGIT (GPIO_ODR_OD3_Msk)

#define DISPLAY_ODR_MASKS   (GPIO_ODR_OD0_Msk  | \
                             GPIO_ODR_OD2_Msk  | \
                             GPIO_ODR_OD7_Msk  | \
                             GPIO_ODR_OD8_Msk  | \
                             GPIO_ODR_OD9_Msk  | \
                             GPIO_ODR_OD11_Msk | \
                             GPIO_ODR_OD13_Msk | \
                             GPIO_ODR_OD15_Msk)

#define DECIMAL_POINT (GPIO_ODR_OD15_Msk)

#define DIGIT_ONE (GPIO_ODR_OD2_Msk | \
                   GPIO_ODR_OD7_Msk)

#define DIGIT_TWO (GPIO_ODR_OD0_Msk  | \
                   GPIO_ODR_OD2_Msk  | \
                   GPIO_ODR_OD13_Msk | \
                   GPIO_ODR_OD9_Msk  | \
                   GPIO_ODR_OD8_Msk)

#define DIGIT_THREE ((DIGIT_TWO ^ GPIO_ODR_OD9_Msk) | GPIO_ODR_OD7_Msk)

#define DIGIT_FOUR (DIGIT_ONE | GPIO_ODR_OD11_Msk | GPIO_ODR_OD13_Msk)

#define DIGIT_FIVE ((DIGIT_THREE ^ GPIO_ODR_OD2_Msk) | GPIO_ODR_OD11_Msk)

#define DIGIT_SIX (DIGIT_FIVE | GPIO_ODR_OD9_Msk)

#define DIGIT_SEVEN (DIGIT_ONE | GPIO_ODR_OD0_Msk)

#define DIGIT_EIGHT (DIGIT_SIX | GPIO_ODR_OD2_Msk)

#define DIGIT_NINE (DIGIT_SEVEN | GPIO_ODR_OD11_Msk | GPIO_ODR_OD13_Msk)

#define DIGIT_ZERO (DIGIT_EIGHT ^ GPIO_ODR_OD13_Msk)