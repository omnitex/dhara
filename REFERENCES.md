## https://github.com/aloebs29/flash_management
Dhara + SPI NAND driver, "minimum implementation". single plane vs dual plane NAND?

## https://github.com/D-Buckingham/NAND_flash
Dhara for NANDs in Zephyr RTOS

## https://forum.buspirate.com/t/2gbit-4gbit-spi-nand-flash-upgrade-chips/205/14
### https://github.com/DangerousPrototypes/BusPirate5-firmware has dhara in src/dhara/
Bus pirate apparently use Dhara? 

### Dual plane devices supported since https://github.com/espressif/idf-extra-components/commit/899be8684499ee4fef7bbb97171182d65104732d ? 

### https://github.com/littlefs-project/ramrsbd
Reed Solomon ECC implementation, not a priority as on-device ECC is (always?) present -- TODO is it mandated by ONFI in any way?