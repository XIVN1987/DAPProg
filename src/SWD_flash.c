/**
 * @file    SWD_flash.c
 * @brief   Program target flash through SWD
 */
#include "swd_host.h"
#include "SWD_flash.h"


extern const program_target_t flash_algo;


error_t target_flash_init(uint32_t flash_start)
{
    if (0 == swd_set_target_state_hw(RESET_PROGRAM)) {
        return ERROR_RESET;
    }

    // Download flash programming algorithm to target and initialise.
    if (0 == swd_write_memory(flash_algo.algo_start, (uint8_t *)flash_algo.algo_blob, flash_algo.algo_size)) {
        return ERROR_ALGO_DL;
    }

    if (0 == swd_flash_syscall_exec(&flash_algo.sys_call_s, flash_algo.init, flash_start, 0, 1, 0)) {
        return ERROR_INIT;
    }

    return ERROR_SUCCESS;
}

error_t target_flash_uninit(void)
{
	swd_flash_syscall_exec(&flash_algo.sys_call_s, flash_algo.uninit, 3, 0, 0, 0);
	
    swd_set_target_state_hw(RESET_RUN);

    swd_off();
    return ERROR_SUCCESS;
}

error_t target_flash_program_page(uint32_t addr, const uint8_t *buf, uint32_t size)
{
    while (size > 0) {
        uint32_t write_size = size > flash_algo.program_buffer_size ? flash_algo.program_buffer_size : size;

        // Write page to buffer
        if (!swd_write_memory(flash_algo.program_buffer, (uint8_t *)buf, write_size)) {
            return ERROR_ALGO_DATA_SEQ;
        }

        // Run flash programming
        if (!swd_flash_syscall_exec(&flash_algo.sys_call_s,
                                    flash_algo.program_page,
                                    addr,
                                    flash_algo.program_buffer_size,
                                    flash_algo.program_buffer,
                                    0)) {
            return ERROR_WRITE;
        }
        
		addr += write_size;
		buf  += write_size;
		size -= write_size;
    }

    return ERROR_SUCCESS;
}

error_t target_flash_erase_sector(uint32_t addr)
{
    if (0 == swd_flash_syscall_exec(&flash_algo.sys_call_s, flash_algo.erase_sector, addr, 0, 0, 0)) {
        return ERROR_ERASE_SECTOR;
    }

    return ERROR_SUCCESS;
}

error_t target_flash_erase_chip(void)
{
    error_t status = ERROR_SUCCESS;

    if (0 == swd_flash_syscall_exec(&flash_algo.sys_call_s, flash_algo.erase_chip, 0, 0, 0, 0)) {
        return ERROR_ERASE_ALL;
    }

    return status;
}
