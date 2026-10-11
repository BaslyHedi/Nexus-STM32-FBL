# Port

What the logic needs from the hardware, stated as interfaces (flash, LEDs, clock, CAN). The STM32 drivers and the host-test fakes both provide them. C++17 has no concepts, so each is a documented set of requirements checked with `static_assert` (SW-03).
