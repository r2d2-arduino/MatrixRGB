#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "hub75/hub75_dma.h"
#include "fonts/tahomabd_10pt.h"     // extern const Font Tahoma10;
#include <initializer_list>

static constexpr Hub75Pins PINS = {
    .r1 = 25, .g1 = 26, .b1 = 27,
    .r2 = 21, .g2 = 22, .b2 = 23,
    .a  = 12, .b  = 16, .c  = 17, .d = 18, .e = -1,
    .clk = 15, .lat = 19, .oe = 5
};

// Юстировочная сетка: по ней видно СРАЗУ всё —
// порядок половин, вертикальный/горизонтальный сдвиг, дёргание фазы
/*static void show_grid(Hub75& p) {
    auto C = [](Colors c) { return GFX_FB<16>::get_color(c); };
    p.fill_screen(C(Colors::BLACK));
    p.fill_rect(0,  0, 64, 8, C(Colors::RED));      // строки 0-7
    p.fill_rect(0,  8, 64, 8, C(Colors::GREEN));    // 8-15  (низ верхней половины)
    p.fill_rect(0, 16, 64, 8, C(Colors::BLUE));     // 16-23 (верх нижней половины)
    p.fill_rect(0, 24, 64, 8, C(Colors::YELLOW));   // 24-31
    for (int x = 0; x < 64; x += 8)                 // вертикальные линии каждые 8 колонок
        p.draw_vline(x, 0, 32, C(Colors::WHITE));
    p.draw_vline(63, 0, 32, C(Colors::WHITE));      // правый край
    p.draw_hline(0,  0, 64, C(Colors::WHITE));      // верх
    p.draw_hline(0, 15, 64, C(Colors::WHITE));      // граница половин (2 линии: 15 и 16)
    p.draw_hline(0, 16, 64, C(Colors::WHITE));
    p.draw_hline(0, 31, 64, C(Colors::WHITE));      // низ
    p.show();
}*/

extern "C" void app_main(void) {
    Hub75Config cfg;
    cfg.pins          = PINS;
    cfg.width         = 64;
    cfg.height        = 32;
    cfg.color_depth   = 4;
    cfg.base_oe_words = 24;
    cfg.double_buffer = true;
    cfg.brightness    = 120;
    cfg.latch_blanking = 2;            // чуть больше гашения вокруг LAT
    cfg.clk_div       = 4;     // word rate 10 МГц → пиксельный CLK 5 МГц
    cfg.pixel_shift   = 0;
    cfg.pre_blank     = 32;

    static Hub75 panel(cfg);
    if (!panel.begin()) return;

    //show_grid(panel);                 // сетка остаётся в буфере на весь скан

    auto C = [](Colors c) { return GFX_FB<16>::get_color(c); };

    panel.fill_screen(C(Colors::BLACK));
    panel.draw_rect(0, 0, 64, 32, C(Colors::WHITE));
    panel.fill_rect( 4, 4, 10, 10, C(Colors::RED));
    panel.fill_rect(16, 4, 10, 10, C(Colors::GREEN));
    panel.fill_rect(28, 4, 10, 10, C(Colors::BLUE));
    panel.draw_line(0, 0, 63, 31, C(Colors::YELLOW));
    panel.draw_text("Arthur+", 4, 40, C(Colors::MAGENTA), tahomabd_10pt);
    panel.draw_text("Hello",   2, 15, Hub75::color8(5),   tahomabd_10pt);
    panel.show();                                   // <- аналог rgb.show()

    int x = 0;
    while (true) {
        panel.fill_rect(0, 56, 64, 8, C(Colors::BLACK));
        panel.fill_rect(x, 58, 6, 4, C(Colors::CYAN));
        panel.show();
        x = (x + 1) % 64;
        ESP_LOGI("APP", "refresh %.0f Hz, heap %u", panel.fps(),
                 (unsigned)heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
        vTaskDelay(pdMS_TO_TICKS(30));
    }
}