#include <string>
#include <chrono>

#include <gatherer.h>

void sleep(const double s) {

  std::this_thread::sleep_for(std::chrono::milliseconds(int(s * 1000)));
}

int main(int argc, char* argv[]) {

  std::string serial_port_file;
  int         baud_rate;
  bool        serial_port_virtual;
  std::string data_path;

  if (argc == 5) {

    serial_port_file    = argv[1];
    baud_rate           = atoi(argv[2]);
    serial_port_virtual = atoi(argv[3]);
    data_path           = argv[4];

    printf("loaded params: \n \
serial port: '%s'\n \
baud rate: '%d'\n \
serial port is: '%s'\n \
output data path: '%s'\n", serial_port_file.c_str(), baud_rate, serial_port_virtual ? "virtual" : "real", data_path.c_str());
  } else {
    printf("params not supplied!\n");
    printf("required: ./gatherer <serial port file> <baud rate> <serial port virtual ? true : false> <output data path>\n");
    return 0;
  }

  // initialize the gatherer
  Gatherer gatherer(data_path);

  // open the serial line
  gatherer.connect(serial_port_file, baud_rate, serial_port_virtual);

  printf("getting status\n");
  gatherer.getStatus();

  printf("powering on\n");
  gatherer.pwr(true);

  /* printf("masking pixel\n"); */
  /* gatherer.maskPixel(10, 20); */

  /* printf("setting threshold"); */
  /* gatherer.setThreshold(333, 555); */

  /* printf("setting configuration preset"); */
  /* gatherer.setConfigurationPreset(2); */

  printf("getting temperature\n");
  gatherer.getTemperature();

  for (int i = 0; i < 10; i++) {

    printf("measuring frame in TOA TOT\n");
    gatherer.measureFrame(1000, LLCP_TPX3_PXL_MODE_TOA_TOT);
  }

  for (int i = 0; i < 10; i++) {

    printf("measuring frame in TOA\n");
    gatherer.measureFrame(1000, LLCP_TPX3_PXL_MODE_TOA);
  }

  for (int i = 0; i < 10; i++) {

    printf("measuring frame in MPX ITOT\n");
    gatherer.measureFrame(1000, LLCP_TPX3_PXL_MODE_MPX_ITOT);
  }

  printf("powering off\n");
  gatherer.pwr(false);

  printf("finished\n");

  return 0;
}
