# Docs

## MiniPIX

### Flashing the FPGA

  0. You need Windows.
  1. Disconnect the MiniPIX USB connector physically from PC and connect it again, wait until it is detected by PC.
  2. Open `cmd.exe` and go to the _FPGA_Programmer_ folder.
  3. execute `program_fpga.bat` file (operation takes a minute or two). It should end with text containing “*successfully*”.
  4. Disconnect and connect the MiniPIX again.

### Progamming the CP

**Dan**: You can use either `arm-none-eabi-gdb` and load the elf file via debugger and attached ST-Link programmer.
Or you can use the ST-Link Utility and program the bin file to the flash.

**Tomas**: I used the st-flash as follows:
```bash
# main.elf -> main.bin
arm-none-eabi-objcopy -O binary minipix.elf minipix.bin

# flash the firmware
st-flash write minipix.bin 0x08000000
```
