# Docs

## System diagram

[![](diagram.png)](./diagram.pdf)

## HW diagram

HW diagram of the Dummy<->STM32F4 example<->Gatherer.

![](hw_diagram_labels.png)

## Commands provided by the MiniPIX Interface

| "command"        | description                               | parameters            | output form                                    |
|------------------|-------------------------------------------|-----------------------|------------------------------------------------|
| `getStatus()`    | gets housekeeping data from MiniPIX       | -                     | a string of text, possibly with numeric values |
| `maskPixel()`    | masks a pixel from further acquiring data | pixel coordinates     | -                                              |
| `getFrame()`     | requests image acquisition                | acquisition time      | packetized image                               |

For consideration

| "command"              | description                                  | parameters       | output form       |
|------------------------|----------------------------------------------|------------------|-------------------|
| `getSinglePxlStream()` | requests image stream of single-pixel events | acquisition time | packetized stream |

## Provided code

* meant to be compiled on Linux, e.g., Ubuntu 20.04 (not older)
* C/C++ built with cmake

## Unresolved questions

**Measurement mode** 

* ToT - Time over Threshold (->Energy)
* ToA - Time of Arrival (->Time)
* both?

**Implementation of the Minipix Interface**

* `llcp.h` and `data_definitions.h` will be in ANSI C
* Minipix Interface: is C++ possible?
  * C++ would be used to make the code well-structured (and cleaner)
  * if possible, C-compatible code would be used, **no high-level libraries**
  * the `minipix interface` would use prototype classes for HW-specific tasks, namely, UART communication, LED signalling
  * prototype classes would be used to manage the acquired and packeted MiniPIX data by the end-user
  * maybe some templating here and there ;-)
