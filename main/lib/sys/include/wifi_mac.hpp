#pragma once

namespace sys
{
    class wifi_mac
    {
        private:
            wifi_mac();

        public:
            wifi_mac(wifi_mac const&) = delete;
            void operator=(wifi_mac const&) = delete;
            static wifi_mac& get_instance();


    };
}

