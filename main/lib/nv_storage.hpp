#pragma once

#include <cstring>
#include <string>
#include <esp_err.h>
#include <nvs_flash.h>

/**
 * These shit must be completely re-written with customised NVS backend and C++ templating
 */
class nv_storage
{
    public:
        nv_storage() : nv_storage("yeet", NVS_READWRITE, "nvs"){};
        nv_storage(const char *nvs_namespace, nvs_open_mode mode) : nv_storage(nvs_namespace, mode, "nvs"){};
        nv_storage(const char *nvs_namespace, nvs_open_mode mode, const char *part_name);

        static int flash_init(const char *part_name = "");
        static int flash_deinit(const char *part_name = "");
        static int flash_format(const char *part_name = "");

        int write(const char *key, uint8_t value);
        int write(const char *key, uint16_t value);
        int write(const char *key, uint32_t value);
        int write(const char *key, uint64_t value);
        int write(const char *key, int8_t value);
        int write(const char *key, int16_t value);
        int write(const char *key, int32_t value);
        int write(const char *key, int64_t value);
        int write(const char *key, const uint8_t *buf, size_t len);
        int write(const char *key, const char *value);
        int write(const char *key, bool value);

        int read(const char *key, uint8_t *value);
        int read(const char *key, uint16_t *value);
        int read(const char *key, uint32_t *value);
        int read(const char *key, uint64_t *value);
        int read(const char *key, int8_t *value);
        int read(const char *key, int16_t *value);
        int read(const char *key, int32_t *value);
        int read(const char *key, int64_t *value);
        int read(const char *key, uint8_t *buf, size_t *len);
        int read(const char *key, char *value, size_t *len);
        int read(const char *key, bool *value);


    private:
        nvs_handle handle = UINT32_MAX;

};