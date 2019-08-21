#include <cstring>

#include <ArduinoJson.h>
#include <wifi_manager.hpp>

#include "rest_controller.hpp"

using namespace web_ctrl;
using namespace sys;

esp_err_t rest_controller::on_wifi_set(httpd_req_t *req)
{
    char buf[req->content_len];
    size_t remain = req->content_len;
    int ret = 0;

    while(remain > 0) {
        if((ret = httpd_req_recv(req, buf, std::min(remain, req->content_len))) <= 0) {
            if(ret == HTTPD_SOCK_ERR_TIMEOUT) {
                continue;
            }
            return ESP_FAIL;
        }

        remain -= ret;
    }

    StaticJsonDocument<192> recv_json;
    if(deserializeJson(recv_json, buf) != DeserializationError::Code::Ok) {
        esp_web_serv::send_body(req, R"({"info":"Invalid request"})", HTTPD_400, HTTPD_TYPE_JSON);
        return ESP_FAIL;
    }

    auto& wifi_mgr = wifi_manager::get_manager();
    std::string ssid = recv_json["ssid"];
    std::string password = recv_json["passwd"];

    if(ssid.empty() || ssid.size() > 32 || password.size() > 64) {
        esp_web_serv::send_body(req, R"({"info":"Invalid SSID or password"})", HTTPD_400, HTTPD_TYPE_JSON);
        return ESP_FAIL;
    }

    if(wifi_mgr.set_sta_config(ssid, password) != ESP_OK) {
        esp_web_serv::send_body(req, R"({"info":"Failed to set WiFi connection"})", HTTPD_500, HTTPD_TYPE_JSON);
        return ESP_FAIL;
    }

    esp_web_serv::send_body(req, R"({"info":"Done"})", HTTPD_200, HTTPD_TYPE_JSON);
    return ESP_OK;
}

esp_err_t rest_controller::on_status_get(httpd_req_t *req)
{
    auto &wifi_mgr = sys::wifi_manager::get_manager();
    StaticJsonDocument<256> json_buf;

    wifi_config_t config;
    if(wifi_mgr.get_sta_config(config) != ESP_OK) {
        esp_web_serv::set_status(req, HTTPD_404);
        esp_web_serv::send_body(req, "");
    }

    json_buf["ssid"] = config.sta.ssid;
    json_buf["sdk"] = esp_get_idf_version();
    json_buf["heap"] = esp_get_free_heap_size();
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
