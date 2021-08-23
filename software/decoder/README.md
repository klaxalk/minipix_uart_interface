# Data decoder and visualizer

This is a small Python example of a decoder and a visualizer of recorded data.
The decoder processes HEXadecimalized frame data (_LLCP_FrameDataMsg_t_) and shows them in a minimalistic GUI.
More info and context in the general [README](../README.md).

**[Data](../gatherer/data/hw_data.txt) stored in the HEXadecimal form.**
![](./fig/data.png)

**Minimalistic GUI showing the recorded images.**
![](./fig/decoder.png)

## Dependencies

The Python dependencies are listed in the _requirements.txt_ file.
The script
```bash
./make_python_env.sh
```
will prepare an isolated virtual environment, that can be used to run the _decoder.py_ by issuing
```bash
./run_in_env.sh
```
