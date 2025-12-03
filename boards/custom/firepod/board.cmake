board_runner_args(openocd "--config=interface/stlink.cfg" "--config=target/stm32l4x.cfg")
include(${ZEPHYR_BASE}/boards/common/openocd.board.cmake)