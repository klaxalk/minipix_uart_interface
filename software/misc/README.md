# Misc

## Virtual serial port on Linux

Install

```bash
sudo apt-get -y install socat
```

Add user to the dialout group (might not be needed in the end)

```bash
sudo gpasswd --add ${USER} dialout
```

Run socat

```bash
socat -d -d PTY,raw,echo=0,link=/tmp/ttyS0 PTY,raw,echo=0,link=/tmp/ttyS1
```

Serial ports appear:
```
/dev/pts/5
/dev/pts/8
```
