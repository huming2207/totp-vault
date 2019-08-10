#include "wifi_mac.hpp"

using namespace sys;

wifi_mac::wifi_mac()
{

}

wifi_mac& wifi_mac::get_instance()
{
    static wifi_mac mac;
    return mac;
}
