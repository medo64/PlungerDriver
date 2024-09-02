#include <xc.h>
#include "app.h"
#include "system.h"
#include "appMain.h"
#include "io.h"
#include "motor.h"
#include "rotary.h"
#include "settings.h"
#include "ssd1306.h"
#include "watchdog.h"
#include "ticker.h"

void main(void) {
    init();
    settings_init();
    watchdog_init();

    ssd1306_init();

    motor_init();
    rotary_init();
    ticker_init();

    ssd1306_writeLine16(" Plunger Driver ");

    execMain();
}
