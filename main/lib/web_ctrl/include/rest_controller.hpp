#pragma once


#include <esp_web_serv.hpp>
#include <esp_err.h>


namespace web_ctrl
{
    class rest_controller
    {
        public:
            rest_controller();

        private:
            sys::esp_web_serv server;
            esp_err_t on_wifi_set(httpd_req_t *req);
            esp_err_t on_status_get(httpd_req_t *req);
            esp_err_t on_totp_set(httpd_req_t *req);
            esp_err_t on_totp_get(httpd_req_t *req);
            esp_err_t on_fp_set(httpd_req_t *req);
            esp_err_t on_fp_get(httpd_req_t *req);
    };
}