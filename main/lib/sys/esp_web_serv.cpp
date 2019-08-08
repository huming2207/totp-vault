#include <esp_http_server.h>
#include "esp_web_serv.hpp"

using namespace sys;

esp_web_serv::esp_web_serv()
{
    httpd_config_t config;
    config.task_priority = tskIDLE_PRIORITY + 5;
    config.stack_size = 8192;
    config.core_id = tskNO_AFFINITY;
    config.server_port = 80;
    config.ctrl_port = 32768;
    config.max_open_sockets = 7;
    config.max_uri_handlers = 8;
    config.max_resp_headers = 8;
    config.backlog_conn = 5;
    config.lru_purge_enable = false;
    config.recv_wait_timeout = 5;
    config.send_wait_timeout = 5;
    config.global_user_ctx = nullptr;
    config.global_user_ctx_free_fn = nullptr;
    config.global_transport_ctx = nullptr;
    config.global_transport_ctx_free_fn = nullptr,
    config.open_fn = nullptr;
    config.close_fn = nullptr;
    config.uri_match_fn = nullptr;

    ESP_ERROR_CHECK(httpd_start(&server, &config));
}

esp_err_t esp_web_serv::on(const std::string& url, const httpd_method_t method,
                            const std::function<esp_err_t(httpd_req_t *)> &cb)
{
    // Register the function to the map
    uri_handler[std::make_tuple(url, method)] = cb;

    // Register an intermediate handler to ESP HTTP server
    httpd_uri_t handler;
    handler.user_ctx = (void *)this;
    handler.uri = url.c_str();
    handler.method = method;
    handler.handler = [](httpd_req_t *req) {
        // Workaround to call outer class private members
        // (remember we can't capture anything with lambda for C-style function pointer)
        auto incoming_req = std::make_tuple(std::string(req->uri), (httpd_method_t)req->method);
        auto *instance = static_cast<esp_web_serv*>(req->user_ctx);
        return instance->uri_handler[incoming_req](req);
    };

    return httpd_register_uri_handler(server, &handler);
}

esp_err_t esp_web_serv::on_error(httpd_err_code_t err_code, const std::function<esp_err_t(httpd_req_t *)> &cb)
{
    // Register error handler
    err_handler[err_code] = cb;

    // Register an intermediate handler to ESP HTTP server
    return httpd_register_err_handler(server, err_code, [](httpd_req_t *req, httpd_err_code_t error){
        auto *instance = static_cast<esp_web_serv*>(req->user_ctx);
        return instance->err_handler[error](req);
    });
}

esp_err_t esp_web_serv::send_body(httpd_req_t *req, const std::vector<uint8_t> &buf)
{
    return httpd_resp_send(req, (const char *)buf.data(), buf.size());
}

esp_err_t esp_web_serv::send_body(httpd_req_t *req, const std::string &buf)
{
    return httpd_resp_sendstr(req, buf.c_str());
}

esp_err_t esp_web_serv::set_header(httpd_req_t *req, const std::map<std::string, std::string> &headers)
{
    esp_err_t ret = ESP_OK;
    for(auto& pair : headers) {
        ret = ret ?: httpd_resp_set_hdr(req, pair.first.c_str(), pair.second.c_str());
    }
    return ret;
}

esp_err_t esp_web_serv::set_header(httpd_req_t *req, const std::string &key, const std::string &val)
{
    return httpd_resp_set_hdr(req, key.c_str(), val.c_str());
}

esp_err_t esp_web_serv::set_status(httpd_req_t *req, const std::string &status)
{
    return httpd_resp_set_status(req, status.c_str());
}

esp_err_t esp_web_serv::set_type(httpd_req_t *req, const std::string &type)
{
    return httpd_resp_set_status(req, type.c_str());
}

esp_err_t esp_web_serv::send_chunk(httpd_req_t *req, const std::vector<uint8_t> &chunk)
{
    return httpd_resp_send_chunk(req, (const char *)chunk.data(), chunk.size());
}

esp_err_t esp_web_serv::send_chunk(httpd_req_t *req, const std::string &chunk)
{
    return httpd_resp_send_chunk(req, chunk.c_str(), chunk.size());
}

httpd_handle_t esp_web_serv::get_serv_instance()
{
    return server;
}
