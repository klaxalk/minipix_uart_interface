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
output data path: '%s'", serial_port_file.c_str(), baud_rate, serial_port_virtual ? "virtual" : "real", data_path.c_str());
  } else {
    printf("params not supplied!\n");
    printf("required: ./gatherer <serial port file> <baud rate> <serial port virtual ? true : false> <output data path>\n");
    return 0;
  }

  // initialize the gatherer
  Gatherer gatherer(data_path);

  // open the serial line
  gatherer.connect(serial_port_file, baud_rate, serial_port_virtual);

  sleep(0.1);

  printf("getting status\n");
  gatherer.getStatus();
  sleep(0.1);

  printf("powering on\n");
  gatherer.pwr(true);
  sleep(0.1);

  /* printf("masking pixel\n"); */
  /* gatherer.maskPixel(10, 20); */
  /* sleep(0.1); */

  /* printf("setting threshold"); */
  /* gatherer.setThreshold(333, 555); */
  /* sleep(0.1); */

  /* printf("setting configuration preset"); */
  /* gatherer.setConfigurationPreset(2); */
  /* sleep(0.1); */

  printf("getting temperature\n");
  gatherer.getTemperature();
  sleep(0.1);

  for (int i = 0; i < 10; i++) {

    gatherer.measureFrame(100);
    sleep(0.1);
  }

  printf("powering off\n");
  gatherer.pwr(false);
  sleep(1.0);

  printf("finished\n");

  return 0;
}
