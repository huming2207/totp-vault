#include <ArduinoJson.h>
#include <wifi_manager.hpp>

#include "rest_controller.hpp"

using namespace web_ctrl;
using namespace net_ctrl;

esp_err_t rest_controller::on_wifi_set(httpd_req_t *req)
{
    return 0;
}

esp_err_t rest_controller::on_status_get(httpd_req_t *req)
{
    auto &wifi_mgr = net_ctrl::wifi_manager::get_manager();
    StaticJsonDocument<256> json_buf;

    wifi_config_t config;
    if(wifi_mgr.get_sta_config(config) != ESP_OK) {
        esp_web_serv::set_status(req, HTTPD_404);
        esp_web_serv::send_body(req, "");
    }

    json_buf["ssid"] = config.sta.ssid;
    json_buf["sdk_ver"] = esp_get_idf_version();
    json_buf["free_heap"] = esp_get_free_heap_size();
    char json_str[128] = { 0 };
    serializeJson(json_buf, json_str);
    esp_web_serv::set_type(req, HTTPD_TYPE_JSON);
    esp_web_serv::send_body(req, json_str);
    return ESP_OK;
}

esp_err_t rest_controller::on_totp_set(httpd_req_t *req)
{
    return 0;
}

esp_err_t rest_controller::on_totp_get(httpd_req_t *req)
{
    return 0;
}

esp_err_t rest_controller::on_fp_set(httpd_req_t *req)
{
    return 0;
}

esp_err_t rest_controller::on_fp_get(httpd_req_t *req)
{
    return 0;
}

rest_controller::rest_controller()
{
    server.on("/status", HTTP_GET, [this](httpd_req_t *req){ return on_status_get(req); });
}
