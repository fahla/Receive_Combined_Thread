# Receive_Combined_Thread
Flashing this to the nrf52840 board makes it a receiver board which receives sensor data send from ccs811, sensirion scd41 and sps30 sensors via thread network. All 4 boards are connected to the same network. The 3 sender boards communicate by sending udp packets to the receiver board via 3 different ports.
