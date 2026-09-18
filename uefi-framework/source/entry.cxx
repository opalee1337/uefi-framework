#include "fw/context.h"

/*
#include "fw/core/crt.h"
#include "fw/core/stl.h"
#include "fw/core/arch.h"

#include "fw/ztils/serial/serial.h"
#include "fw/ztils/pe_util/pe_util.h"
#include "fw/ztils/detour_hook/detour_hook.h"
#include "fw/ztils/pattern_scan/pattern_scan.h"
*/

efi_status_t EfiMain(efi_handle image_handle, efi::system_table* system_table) {
    g_ctx.init(image_handle, system_table);


    // add ur code here, test wahtever you need everything works :)


    return efi::status_codes::success;
}