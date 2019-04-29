#include "nv_storage.hpp"

nv_storage::nv_storage(const char *nvs_namespace, nvs_open_mode mode, const char *part_name)
{
    ESP_ERROR_CHECK(nvs_open_from_partition(part_name, nvs_namespace, mode, &handle));
}

int nv_storage::write(const char *key, const uint8_t value)
{
    return nvs_set_u8(handle, key, value);
}

int nv_storage::write(const char *key, const uint16_t value)
{
    return nvs_set_u16(handle, key, value);
}

int nv_storage::write(const char *key, const uint32_t value)
{
    return nvs_set_u32(handle, key, value);
}

int nv_storage::write(const char *key, const uint64_t value)
{
    return nvs_set_u64(handle, key, value);
}

int nv_storage::write(const char *key, const int8_t value)
{
    return nvs_set_i8(handle, key, value);
}

int nv_storage::write(const char *key, const int16_t value)
{
    return nvs_set_i16(handle, key, value);
}

int nv_storage::write(const char *key, const int32_t value)
{
    return nvs_set_i32(handle, key, value);
}

int nv_storage::write(const char *key, const int64_t value)
{
    return nvs_set_i64(handle, key, value);
}

int nv_storage::write(const char *key, const uint8_t *buf, size_t len)
{
    return nvs_set_blob(handle, key, buf, len);
}

int nv_storage::write(const char *key, const char *value)
{
    return nvs_set_str(handle, key, value);
}

int nv_storage::write(const char *key, bool value)
{
    return nvs_set_u8(handle, key, value ? 1 : 0);
}

int nv_storage::read(const char *key, uint8_t *value)
{
    return nvs_get_u8(handle, key, value);
}

int nv_storage::read(const char *key, uint16_t *value)
{
    return nvs_get_u16(handle, key, value);
}

int nv_storage::read(const char *key, uint32_t *value)
{
    return nvs_get_u32(handle, key, value);
}

int nv_storage::read(const char *key, uint64_t *value)
{
    return nvs_get_u64(handle, key, value);
}

int nv_storage::read(const char *key, int8_t *value)
{
    return nvs_get_i8(handle, key, value);
}

int nv_storage::read(const char *key, int16_t *value)
{
    return nvs_get_i16(handle, key, value);
}

int nv_storage::read(const char *key, int32_t *value)
{
    return nvs_get_i32(handle, key, value);
}

int nv_storage::read(const char *key, int64_t *value)
{
    return nvs_get_i64(handle, key, value);
}

int nv_storage::read(const char *key, uint8_t *buf, size_t *len)
{
    return nvs_get_blob(handle, key, buf, len);
}

int nv_storage::read(const char *key, char *value, size_t *len)
{
    return nvs_get_str(handle, key, value, len);
}

int nv_storage::read(const char *key, bool *value)
{
    uint8_t result = UINT8_MAX;
    int ret = 0;

    if((ret = nvs_get_u8(handle, key, &result)) != ESP_OK) return ret;
    *value = result == 1;

    return ESP_OK;
}

int nv_storage::flash_init(const char *part_name)
{
    const char * name = std::strlen(part_name) < 1 ? "nvs" : part_name;

    esp_err_t err = nvs_flash_init_partition(name);
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }

    return err;
}

int nv_storage::flash_deinit(const char *part_name)
{
    const char * name = std::strlen(part_name) < 1 ? "nvs" : part_name;
    return nvs_flash_deinit_partition(name);
}

int nv_storage::flash_format(const char *part_name)
{
    const char * name = std::strlen(part_name) < 1 ? "nvs" : part_name;
    return nvs_flash_erase_partition(name);
}

