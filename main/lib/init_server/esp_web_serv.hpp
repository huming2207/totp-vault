#pragma once

#include <string>
#include <functional>
#include <map>

#include <esp_http_server.h>

class esp_web_serv
{
    public:
        esp_web_serv();
        esp_err_t on(const std::string& url, httpd_method_t method,
                        const std::function<esp_err_t(httpd_req_t*)>& cb);
        esp_err_t on_error(httpd_err_code_t err_code, const std::function<esp_err_t(httpd_req_t*)>& cb);

    private:
        std::map<std::tuple<std::string, httpd_method_t>, std::function<esp_err_t(httpd_req_t*)>> uri_handler;
        std::map<httpd_err_code_t, std::function<esp_err_t(httpd_req_t*)>> err_handler;
        httpd_handle_t server = nullptr;
};