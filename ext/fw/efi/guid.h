#pragma once

#include <cstdint>

namespace efi {

    // https://uefi.org/specs/UEFI/2.11/Apx_A_GUID_and_Time_Formats.html
    struct guid {
        uint32_t data1;
        uint16_t data2;
        uint16_t data3;
        uint8_t  data4[8];

        constexpr bool operator==(const guid& o) const {
            return data1 == o.data1
                && data2 == o.data2
                && data3 == o.data3
                && data4[0] == o.data4[0] && data4[1] == o.data4[1]
                && data4[2] == o.data4[2] && data4[3] == o.data4[3]
                && data4[4] == o.data4[4] && data4[5] == o.data4[5]
                && data4[6] == o.data4[6] && data4[7] == o.data4[7];
        }

        constexpr bool operator!=(const guid& o) const { return !(*this == o); }
    };

    consteval guid make_guid(
        uint32_t d1, uint16_t d2, uint16_t d3,
        uint8_t d4_0, uint8_t d4_1, uint8_t d4_2, uint8_t d4_3,
        uint8_t d4_4, uint8_t d4_5, uint8_t d4_6, uint8_t d4_7
    ) {
        return { d1, d2, d3, { d4_0, d4_1, d4_2, d4_3, d4_4, d4_5, d4_6, d4_7 } };
    }

}

using efi_guid = efi::guid;