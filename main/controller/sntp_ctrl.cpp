#include <esp_sntp.h>
#include "sntp_ctrl.hpp"

void sntp_ctrl::sync_time()
{
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_setservername(1, "time.apple.com");
    sntp_set_sync_mode(SNTP_SYNC_MODE_IMMED);
    sntp_init();
    while(sntp_get_sync_status() != SNTP_SYNC_STATUS_COMPLETED) {
        vTaskDelay(500);
    }
}

void sntp_ctrl::connect_ap()
{

}
