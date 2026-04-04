#ifndef UI_BUTTON_H
#define UI_BUTTON_H

#include <stdbool.h>
#include "../Struct.h"

void ScaleButton(Button *btn, float ratioX, float ratioY);

bool isClicked(const Button *btn, int x, int y);

#endif 