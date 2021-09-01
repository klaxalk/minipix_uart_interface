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

    printf(
        "loaded params: \n \
serial port: '%s'\n \
baud rate: '%d'\n \
serial port is: '%s'\n \
output data path: '%s'\n",
        serial_port_file.c_str(), baud_rate, serial_port_virtual ? "virtual" : "real", data_path.c_str());
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

  /* printf("masking pixels\n"); */
  /* int square_size = 20; */
  /* for (int i = 128 - int(square_size / 2.0); i < 128 + int(square_size / 2.0); i++) { */
  /*   for (int j = 245 - int(square_size / 2.0); j < 245 + int(square_size / 2.0); j++) { */

  /*     bool state = true; */

  /*     gatherer.maskPixel(i, j, state); */
  /*     printf("%s pixel x: %d, y: %d\n", state ? "masking" : "unmasking", i, j); */
  /*   } */
  /* } */

  /* printf("setting threshold"); */
  /* gatherer.setThreshold(333, 555); */

  /* printf("setting configuration preset 0\n"); */
  /* gatherer.setConfigurationPreset(0); */

  /* printf("setting configuration preset 1\n"); */
  /* gatherer.setConfigurationPreset(1); */

  printf("getting temperature\n");
  gatherer.getTemperature();

  for (int i = 0; i < 10; i++) {

    printf("measuring frame in TOA TOT\n");
    gatherer.measureFrame(100, LLCP_TPX3_PXL_MODE_TOA_TOT);
    sleep(0.01);
  }

  for (int i = 0; i < 10; i++) {

    printf("measuring frame in TOA\n");
    gatherer.measureFrame(100, LLCP_TPX3_PXL_MODE_TOA);
    sleep(0.01);
  }

  for (int i = 0; i < 10; i++) {

    printf("measuring frame in MPX ITOT\n");
    gatherer.measureFrame(100, LLCP_TPX3_PXL_MODE_MPX_ITOT);
    sleep(0.01);
  }

  printf("powering off\n");
  gatherer.pwr(false);

  // this will stop the threads and disconnect the uart
  gatherer.stop();

  printf("finished\n");

  return 0;
}
