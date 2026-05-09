# AT_Command Library

![GitHub release (latest by date)](https://img.shields.io/github/v/release/akkoyun/AT_Command) ![arduino-library-badge](https://www.ardu-badge.com/badge/AT_Command.svg?) ![Visits Badge](https://badges.pufler.dev/visits/akkoyun/AT_Command) ![GitHub stars](https://img.shields.io/github/stars/akkoyun/AT_Command?style=flat&logo=github)

Type-safe, memory-efficient C++ AT command library for **Telit LE910C1-EUX** and **GE910** series GSM/LTE modules. Covers modem control, SIM, network registration, TCP/IP sockets, WebSocket (ws:// and wss://), HTTP, FTP, and diagnostics — all behind a clean inheritance-based API.

---

## Table of Contents

- [Supported Hardware](#supported-hardware)
- [Installation](#installation)
- [Architecture](#architecture)
- [Memory Configuration](#memory-configuration)
- [Quick Start](#quick-start)
- [API Reference](#api-reference)
  - [Modem Control](#modem-control)
  - [Identification](#identification)
  - [SIM](#sim)
  - [Network Registration](#network-registration)
  - [PDP Context](#pdp-context)
  - [GPIO / LED](#gpio--led)
  - [Network Diagnostics](#network-diagnostics)
  - [Socket Configuration](#socket-configuration)
  - [TCP Socket I/O](#tcp-socket-io)
  - [WebSocket](#websocket)
  - [FTP](#ftp)
  - [System](#system)
- [Constants Reference](#constants-reference)
- [Configuration](#configuration)
- [Examples](#examples)

---

## Supported Hardware

| Module | Family | Band |
|--------|--------|------|
| LE910C1-EUX | LTE Cat.1 | EU (B1/B3/B7/B8/B20/B28) |
| GE910-QUAD | GSM/GPRS | Quad-band 850/900/1800/1900 |
| GE910-QUAD-V3 | GSM/GPRS | Quad-band |
| GE910-GNSS | GSM/GPRS + GPS | Quad-band |
| LE910S1-EA / EAG | LTE Cat.1 | — |
| LE910R1-EU | LTE Cat.1 | EU |

---

## Installation

### Arduino IDE

1. Sketch → Include Library → Manage Libraries…
2. Search **AT_Command** → Install

### PlatformIO

```ini
lib_deps = akkoyun/AT_Command
```

### Manual

Clone or download into your Arduino `libraries/` folder:

```
libraries/AT_Command/
├── src/
│   ├── AT_Command.h
│   ├── Config.h
│   └── Definations.h
├── examples/
└── README.md
```

---

## Architecture

All AT command functions are **protected** — they are not callable on a raw `LE910C1_EUX` object. This forces you to subclass it, creating a clean API boundary and letting you expose only the commands your application actually uses.

```cpp
#include <AT_Command.h>

class Modem : public LE910C1_EUX {
public:
    explicit Modem(Stream &s) : LE910C1_EUX(s) {}

    bool init() {
        AT();
        ATE(false);   // echo off
        CMEE(1);      // numeric error codes
        FCLASS(0);    // data mode
        return true;
    }

    bool connect(const char * apn) {
        uint8_t pin = 0;
        if (!CPIN(pin) || pin != _SIM_READY_) return false;
        CGDCONT(1, "IP", apn);
        uint8_t ip[4] = {};
        return SGACT(1, true, ip);
    }
};

Modem modem(Serial1);

void setup() {
    Serial1.begin(115200);
    modem.init();
    modem.connect("internet");
}
```

### Shared IO Buffer

Every AT function reuses a single `char _IO_Buffer[_IO_Buffer_Size]` member instead of allocating per-function stack buffers. This eliminates stack overflow risk on AVR targets and keeps RAM usage predictable.

---

## Memory Configuration

The IO buffer size defaults to **1024 bytes** and is the library's primary SRAM cost. Override it before `#include` to reduce memory:

```cpp
// In your sketch or a config header — must come before AT_Command.h
#define _IO_Buffer_Size 128   // for basic init-only usage
#define _IO_Buffer_Size 256   // for most TCP/socket operations
#define _IO_Buffer_Size 512   // for SRECV / WebSocket payloads
// (default 1024 — required for FTPRECV with large chunks)
#include <AT_Command.h>
```

**Minimum size guide:**

| Use case | Minimum `_IO_Buffer_Size` |
|----------|--------------------------|
| Init only (AT, ATE, CMEE, CPIN…) | 64 |
| Network + TCP sockets | 256 |
| HTTP / WebSocket payloads | 512 |
| FTP receive (large chunks) | 1024 |

> All string literals use `F()` (stored in flash, not SRAM). All numeric constants are `constexpr` (zero SRAM cost). Functions not called by your code are dead-code-eliminated by the linker.

---

## Quick Start

### Modem Initialisation

```cpp
AT();           // verify modem is alive
ATE(false);     // turn off echo
CMEE(1);        // numeric +CME ERROR codes
FCLASS(0);      // data connection mode
```

### Network + IP

```cpp
uint8_t pin = 0;
CPIN(pin);                          // check SIM ready
CGDCONT(1, "IP", "internet");       // set APN
uint8_t ip[4] = {};
SGACT(1, true, ip);                 // activate PDP context
// ip[] now holds assigned IP address
```

### TCP Socket

```cpp
SCFG(1, 1, 300, 90, 600, 50);
SCFGEXT(1, 1, false, 0, false, false);
ATSD(1, 0, "93.184.216.34", 80, 255, 0, 1);  // connect
SSEND(1, _HTTP_GET_, nullptr);                // send GET
char buf[256];
SRECV(1, 256, buf);                           // receive
SH(1);                                        // close
```

### WebSocket

```cpp
WSOPEN(1, "echo.websocket.events", 80, "/");
WSSEND(1, "{\"hello\":1}");
char buf[256]; uint8_t op = 0;
WSRECV(1, buf, sizeof(buf), op);
WSCLOSE(1);
```

---

## API Reference

All functions return `bool` (true = success) unless noted otherwise. Output parameters are passed by reference.

---

### Modem Control

#### `AT()`

```
--> AT\r\n
<-- \r\nOK\r\n
```

Sends the attention command. Use to verify the modem is alive and the UART link is working.

---

#### `ATE(bool echo = false)`

```
--> ATE0\r\n  or  ATE1\r\n
<-- \r\nOK\r\n
```

Enable (`true`) or disable (`false`) command echo. Disable echo before reading responses programmatically.

---

#### `FCLASS(uint8_t n = 0)`

```
--> AT+FCLASS=<n>\r\n
<-- \r\nOK\r\n
```

Set connection mode. `0` = data (default for IoT). `8` = fax.

---

#### `CFUN(uint8_t fun)`

```
--> AT+CFUN=<fun>\r\n
<-- \r\nOK\r\n
```

Set phone functionality. `1` = full, `0` = minimum, `4` = airplane (RF off).

---

#### `CMEE(uint8_t n = 1)`

```
--> AT+CMEE=<n>\r\n
<-- \r\nOK\r\n
```

Extended error reporting. `0` = off, `1` = numeric `+CME ERROR: N`, `2` = verbose text.

---

#### `CEER(uint16_t & code)`

```
--> AT#CEER\r\n
<-- \r\n#CEER: <code>\r\n\r\nOK\r\n
```

Telit proprietary extended error code for the most recent failure. Useful after a failed `SGACT` or socket operation.

---

#### `K(uint8_t k = 0)`

```
--> AT&K<n>\r\n
<-- \r\nOK\r\n
```

Hardware flow control. `0` = none, `3` = RTS/CTS, `4` = XON/XOFF.

---

#### `Z(uint8_t profile)`

```
--> ATZ<n>\r\n
<-- \r\nOK\r\n
```

Soft reset / restore factory profile. `0` = profile 0, `1` = profile 1.

---

#### `SHDN()`

```
--> AT#SHDN\r\n
<-- \r\nOK\r\n
```

Graceful network detach followed by modem power down.

---

### Identification

#### `CGMI(uint8_t & manufacturer)`

```
--> AT+CGMI\r\n
<-- \r\n<name>\r\n\r\nOK\r\n
```

Returns `_MODEM_MANUFACTURER_TELIT_` or `_MODEM_MANUFACTURER_UNKNOWN_`.

---

#### `CGMM(uint8_t & model)`

```
--> AT+CGMM\r\n
<-- \r\n<model>\r\n\r\nOK\r\n
```

Returns one of `_MODEM_MODEL_LE910C1_EUX_`, `_MODEM_MODEL_GE910_QUAD_`, etc.

---

#### `SWPKGV(char * firmware)`

```
--> AT#SWPKGV\r\n
<-- \r\n<fw_ver>\r\n...\r\nOK\r\n
```

Firmware version string written into `firmware` (caller must allocate ≥ 15 bytes).

---

#### `CGSN(char * imei)`

```
--> AT+CGSN=1\r\n
<-- \r\n+CGSN: <imei>\r\n\r\nOK\r\n
```

15-digit IMEI string written into `imei` (caller must allocate ≥ 16 bytes).

---

### SIM

#### `CPIN(uint8_t & code)`

```
--> AT+CPIN?\r\n
<-- \r\n+CPIN: READY|SIM PIN|SIM PUK\r\n\r\nOK\r\n
```

SIM status. `code` is set to one of:

| Constant | Value | Meaning |
|----------|-------|---------|
| `_SIM_READY_` | — | SIM ready, no PIN needed |
| `_SIM_PIN_` | — | PIN required |
| `_SIM_PUK_` | — | PUK required |
| `_SIM_UNKNOWN_` | — | Unknown / not inserted |

Returns `true` only when `_SIM_READY_`.

---

#### `CCID(char * iccid)`

```
--> AT#CCID\r\n
<-- \r\n#CCID: <iccid>\r\n\r\nOK\r\n
```

20-digit ICCID string written into `iccid` (caller must allocate ≥ 21 bytes).

---

#### `SIMDET(bool type, uint8_t mode, bool & simPresent)`

```
--> AT#SIMDET=<mode>\r\n          (type = _AT_SET_)
--> AT#SIMDET?\r\n                (type = _AT_GET_)
<-- \r\nOK\r\n  /  \r\n#SIMDET: <mode>,<state>\r\n\r\nOK\r\n
```

SIM hot-swap detection. `mode`: 0 = disable, 1 = enable, 2 = auto.

---

### Network Registration

#### `CREG(bool type, uint8_t & mode, uint8_t & stat)`

```
--> AT+CREG=<mode>\r\n            (type = _AT_SET_)
--> AT+CREG?\r\n                  (type = _AT_GET_)
<-- \r\n+CREG: <mode>,<stat>\r\n\r\nOK\r\n
```

`stat` values: `0` = not registered, `1` = home, `2` = searching, `3` = denied, `5` = roaming.

---

#### `WS46(bool type, uint8_t & mode)`

```
--> AT+WS46?\r\n   /   AT+WS46=<mode>\r\n
<-- \r\n+WS46: <mode>\r\n\r\nOK\r\n
```

Wireless technology selection. Returned in `_Connection_2G_`, `_Connection_3G_`, `_Connection_4G_`.

| WS46 code | Constant |
|-----------|----------|
| 12 | `_CONNECTION_2G_` |
| 22, 29 | `_CONNECTION_3G_` |
| 25, 28, 30, 31 | `_CONNECTION_4G_` |

---

#### `RFSTS(uint8_t connType, uint16_t & mcc, uint16_t & mnc, uint16_t & rssi, uint8_t & signal, uint16_t & tac, uint32_t & cid)`

```
--> AT#RFSTS\r\n
<-- \r\n#RFSTS: "<MCC MNC>",<EARFCN>,<RSRP>,<RSSI>,<RSRQ>,<TAC>,,<TXPWR>,...,<CID>,...\r\n\r\nOK\r\n
```

4G-only (pass `_CONNECTION_4G_`). Returns PLMN codes, RSSI absolute value (dBm), signal quality 0–5, TAC, and Cell ID.

---

#### `CSQ(uint16_t & rssi)`

```
--> AT+CSQ\r\n
<-- \r\n+CSQ: <rssi>,<ber>\r\n\r\nOK\r\n
```

Signal quality (3GPP TS 27.007 §8.5). `rssi` is returned as the absolute dBm value:

| CSQ | RSSI |
|-----|------|
| 0 | 113 (≤ −113 dBm) |
| 1 | 111 |
| 2–30 | 113 − CSQ×2 |
| 31 | 51 (−51 dBm) |
| 99 | 0 (unknown) |

---

#### `CCLK(uint8_t & year, month, day, hour, minute, second, timeZone)`

```
--> AT+CCLK?\r\n
<-- \r\n+CCLK: "YY/MM/DD,HH:MM:SS±ZZ"\r\n\r\nOK\r\n
```

RTC. Timezone in quarter-hours; negative offset encoded as two's complement in `timeZone`.

---

#### `CTZU(bool enable = true)`

Automatic timezone update from network. `true` = on.

#### `NITZ(uint8_t val = 1, bool mode = 1)`

Network time unsolicited reporting. `val` bitmask: bit0=time, bit1=DST, bit2=TZ.

#### `CCLKMODE(bool mode = 0)`

Clock mode. `0` = UTC, `1` = local time.

---

### PDP Context

#### `CGDCONT(uint8_t cid, const char * type, const char * apn)`

```
--> AT+CGDCONT=<cid>,"<type>","<apn>"\r\n
<-- \r\nOK\r\n
```

Define PDP context. `cid` = 1–6, `type` = `"IP"` or `"IPV6"`.

```cpp
CGDCONT(1, "IP", "internet");
```

---

#### `SGACT(uint8_t cid, bool activate, uint8_t ip[4])`

```
--> AT#SGACT=<cid>,<1|0>\r\n
<-- \r\n#SGACT: <ip>\r\n\r\nOK\r\n
```

Activate (`true`) or deactivate (`false`) PDP context. On activation, writes the assigned IP into `ip[4]`.

```cpp
uint8_t ip[4] = {};
if (SGACT(1, true, ip)) {
    // ip[0].ip[1].ip[2].ip[3] = assigned address
}
```

---

### GPIO / LED

#### `GPIO(bool type, uint8_t pin, uint8_t mode, uint8_t dir)`

```
--> AT#GPIO=<pin>,<mode>,<dir>\r\n
<-- \r\nOK\r\n
```

Control GPIO pin. `dir`: 0 = input, 1 = output, 2 = alternate.

#### `SLED(uint8_t n = 2)`

Status LED mode. `0` = off, `1` = on, `2` = network-blink, `3` = software-controlled.

#### `SLEDSAV()`

Save LED setting to NVM (survives reboot).

#### `E2SLRI(uint16_t pulseDuration = 50)`

Ring indicator pulse duration on incoming socket event. 50–1150 ms.

---

### Network Diagnostics

#### `FRWL(bool type, uint8_t action, const char * ip)`

```
--> AT#FRWL=<action>[,"<ip>","255.255.255.255"]\r\n
<-- \r\nOK\r\n
```

IP firewall rule. `action`: `0` = add, `1` = remove, `2` = remove all.

#### `ICMP(uint8_t mode = 2)`

ICMP ping enable. `0` = off, `1` = on, `2` = on + unsolicited.

#### `Ping(const char * ip, uint16_t & time)`

```
--> AT#PING="<ip>",1\r\n
<-- \r\n#PING: 01,"<ip>",0,<time_ms>\r\n\r\nOK\r\n
```

Single ICMP ping. Returns round-trip time in ms.

#### `DNS(uint8_t cid, const char * primary, const char * secondary)`

Set manual DNS servers for a PDP context.

#### `CACHEDNS(bool enable)`

DNS response caching. `true` = on.

---

### Socket Configuration

Configure sockets **before** opening a connection. Settings persist until power cycle.

#### `SCFG(uint8_t connID, uint8_t cid, uint16_t pktSz, uint16_t maxTo, uint16_t connTo, uint8_t txTo)`

```
--> AT#SCFG=<connID>,<cid>,<pktSz>,<maxTo>,<connTo>,<txTo>\r\n
<-- \r\nOK\r\n
```

| Parameter | Description | Typical value |
|-----------|-------------|---------------|
| `connID` | Socket ID 1–6 | 1 |
| `cid` | PDP context ID | 1 |
| `pktSz` | Packet size (0–1500 bytes) | 300 |
| `maxTo` | Inactivity timeout (0–65535 s, 0=off) | 90 |
| `connTo` | Connection timeout (10–1200 s) | 600 |
| `txTo` | Transmit delay (0–255 × 100 ms) | 50 |

#### `SCFGEXT(uint8_t connID, uint8_t srMode, bool recvDataMode, uint8_t keepAlive, bool listenAutoRsp, bool sendDataMode)`

Extended config. `srMode=1` enables `#SRING` unsolicited data notification. `keepAlive` = TCP keepalive interval in minutes (0 = off).

#### `SCFGEXT2` / `SCFGEXT3`

Further extended config for buffering, closure behaviour, and UDP mode. See header for parameter details.

#### `TCPMAXWIN(uint16_t winSize = 0, uint8_t scaleFactor = 0)`

TCP window size (0 = default). Increase for high-throughput connections.

#### `SKTTO(uint16_t timeout = 90)`

Socket inactivity timeout in seconds. `0` = never close automatically.

---

### TCP Socket I/O

#### `ATSD(uint8_t connID, bool proto, const char * ip, uint8_t port, uint8_t closureType, uint16_t iPort, bool connMode)`

```
--> AT#SD=<connID>,<proto>,<port>,"<ip>",<closureType>,<iPort>,<connMode>\r\n
<-- \r\nOK\r\n
```

Open a TCP (`proto=0`) or UDP (`proto=1`) socket. `connMode=1` = command mode (required for `AT#SSEND`/`AT#SRECV`).

```cpp
ATSD(1, 0, "93.184.216.34", 80, 255, 0, 1);
```

---

#### `SSEND(uint8_t connID, uint8_t method, const char * data)`

```
--> AT#SSEND=<connID>\r\n
     > <HTTP-header><data><Ctrl-Z>
<-- \r\nOK\r\n
```

Send data. `method` selects the HTTP verb and header:

| Constant | Header generated |
|----------|-----------------|
| `_HTTP_POST_` | `POST <endpoint> HTTP/1.1\r\nHost: ...\r\nContent-Length: N\r\n...` |
| `_HTTP_GET_` | `GET <endpoint> HTTP/1.1\r\nHost: ...\r\n...` |
| `_HTTP_RESPONSE_` | `HTTP/1.1 200 OK\r\nConnection: close\r\n...` |

Three overloads available: callback-based, config-macro server, explicit server+endpoint.

---

#### `SRECV(uint8_t connID, uint16_t maxByte, char * data)`

```
--> AT#SRECV=<connID>,<maxByte>\r\n
<-- \r\n#SRECV: <connID>,<n>\r\n<data>\r\nOK\r\n
```

Receive up to `maxByte` bytes from socket buffer. Returns `false` if buffer is empty.

---

#### `SRING(uint16_t & length)` / `SRING()`

Wait for an unsolicited `#SRING` notification (data available on socket). The first overload (50 s timeout) returns the data length; the second (1 s timeout) returns a bool indicating whether any SRING was seen.

---

#### `SH(uint8_t connID)`

Close socket (hang up).

#### `SL(uint8_t connID, bool listen, uint16_t port, uint8_t closureType)`

Start (`listen=true`) or stop listening on a port.

#### `SA(uint8_t connID, uint8_t connMode, uint16_t & length)`

Accept an incoming connection on a listening socket.

#### `SS(uint8_t connID, uint8_t & state)`

Query socket state. `state` values:

| Value | Constant | Meaning |
|-------|----------|---------|
| 0 | `_SOCKET_CLOSED_` | Closed |
| 1 | — | Listen |
| 2 | — | Established |
| 3 | — | Active |
| 4 | — | Remote closed |
| 5 | — | Idle |

#### `SO(uint8_t connID)`

Reopen a previously configured socket.

#### `SI(uint8_t connID, uint16_t & buffered)`

Socket info — returns bytes buffered (available to read).

---

### WebSocket

WebSocket support implements RFC 6455 over the existing TCP/SSL socket infrastructure. All functions accept an optional `_SSL = false` parameter — pass `true` for `wss://` connections.

#### `WSOPEN(uint8_t connID, const char * host, uint16_t port, const char * path, bool ssl = false)`

Opens a TCP (or TLS) connection then performs the HTTP/1.1 WebSocket Upgrade handshake. Polls for `101 Switching Protocols` with 500 ms retries up to 15 s.

```cpp
// Plain ws://
WSOPEN(1, "echo.websocket.events", 80, "/");

// Secure wss://
WSOPEN(1, "echo.websocket.events", 443, "/", true);
```

---

#### `WSSEND(uint8_t connID, const char * data, bool ssl = false)`

Sends a masked text frame. Supports payloads up to 65535 bytes (uses 16-bit extended length field for payloads > 125 bytes). The masking key is chosen to ensure no wire byte equals `0x1A` (the `AT#SSEND` Ctrl-Z terminator).

```cpp
WSSEND(1, "{\"type\":\"hello\",\"v\":1}");
```

---

#### `WSRECV(uint8_t connID, char * data, uint16_t maxLen, uint8_t & opcode, bool ssl = false)`

Reads one server frame. Parses 7-bit and 16-bit extended length fields. `opcode` is set to one of:

| Constant | Value | Meaning |
|----------|-------|---------|
| `_WS_OPCODE_CONT_` | 0 | Continuation |
| `_WS_OPCODE_TEXT_` | 1 | Text payload |
| `_WS_OPCODE_BINARY_` | 2 | Binary payload |
| `_WS_OPCODE_CLOSE_` | 8 | Close frame |
| `_WS_OPCODE_PING_` | 9 | Ping (must reply with WSPONG) |
| `_WS_OPCODE_PONG_` | 10 | Pong reply |

```cpp
char buf[256];
uint8_t op = 0;
if (WSRECV(1, buf, sizeof(buf), op)) {
    if (op == _WS_OPCODE_PING_) {
        WSPONG(1);                  // RFC 6455 §5.5.2 — mandatory
    } else if (op == _WS_OPCODE_TEXT_) {
        Serial.println(buf);
    }
}
```

---

#### `WSPING(uint8_t connID, bool ssl = false)`

Sends a masked ping control frame. The server must reply with a pong.

#### `WSPONG(uint8_t connID, bool ssl = false)`

Sends a masked pong reply. **Must** be called when `WSRECV` returns `opcode == _WS_OPCODE_PING_` — failure to do so will cause the server to close the connection (RFC 6455 §5.5.2).

#### `WSCLOSE(uint8_t connID, bool ssl = false)`

Sends a masked close frame then closes the underlying TCP/SSL socket.

#### Complete WebSocket Example

```cpp
// Connect
if (!WSOPEN(1, "echo.websocket.events", 80, "/")) return;

// Send
WSSEND(1, "{\"hello\":1}");

// Receive (with ping-pong handling)
char buf[256];
uint8_t op = 0;
if (WSRECV(1, buf, sizeof(buf), op)) {
    if      (op == _WS_OPCODE_PING_)   WSPONG(1);
    else if (op == _WS_OPCODE_TEXT_)   Serial.println(buf);
}

// Client-initiated keepalive
if (WSPING(1)) {
    if (WSRECV(1, buf, sizeof(buf), op) && op == _WS_OPCODE_PONG_)
        Serial.println("pong ok");
}

// Close
WSCLOSE(1);
```

---

#### SSL / wss:// Notes

SSL is configured with cipher-suite auto-negotiation and no certificate verification (`authMode=0`), which is suitable for IoT devices that cannot store a certificate chain. The underlying Telit commands used are `AT#SSLCFG`, `AT#SSLSECCFG`, `AT#SSLD`, `AT#SSLSEND`, `AT#SSLRECV`, and `AT#SSLH`.

---

### FTP

FTP functions use the Telit built-in FTP client. Typical flow: `FTPOPEN` → `FTPTYPE` → `FTPCWD` → `FTPGETPKT` → (loop) `FTPRECV` → `FTPCLOSE`.

#### `FTPOPEN(const char * server, const char * user, const char * password, uint8_t mode)`

```
--> AT#FTPOPEN="<server>","<user>","<pass>",<mode>\r\n
```

Open FTP session. `mode`: `0` = active, `1` = passive (recommended behind NAT).

#### `FTPTYPE(uint8_t type)`

Transfer type. `0` = binary, `1` = ASCII. Use binary for firmware/hex files.

#### `FTPCWD(const char * folder)`

Change working directory.

#### `FTPFSIZE(const char * filename, uint32_t & length)`

Get remote file size in bytes.

#### `FTPGETPKT(const char * filename, uint8_t viewMode)`

Start packet-mode download. `viewMode`: `0` = binary, `1` = text. Data is not returned here — it is queued in the modem's buffer for retrieval with `FTPRECV`.

#### `FTPRECV(uint16_t size, char * data)`

```
--> AT#FTPRECV=<size>\r\n
<-- \r\n#FTPRECV: <size>\r\n<data>\r\nOK\r\n
```

Retrieve `size` bytes from the modem's FTP buffer. **`data` must be at least `size + 30` bytes** to accommodate the response header and footer.

```cpp
// Example: download 200 bytes at a time
char chunk[230];  // 200 + 30 overhead
while (FTPRECV(200, chunk)) {
    // process chunk...
}
```

#### `FTPCLOSE()`

Close FTP session.

#### `FTPTO(uint16_t timeout)`

Set FTP session timeout in seconds.

---

### System

#### `ICMP(uint8_t mode = 2)`

Enable ICMP ping replies. Required before `Ping()`.

#### `Ping(const char * ip, uint16_t & time)`

Single ICMP ping. Returns round-trip time in ms. Requires `ICMP(2)` to be set.

```cpp
ICMP(2);
uint16_t rtt = 0;
if (Ping("8.8.8.8", rtt)) Serial.println(rtt);
```

---

## Constants Reference

### Connection / SIM state

```cpp
_SIM_READY_       // CPIN: SIM ready
_SIM_PIN_         // CPIN: PIN required
_SIM_PUK_         // CPIN: PUK required
_SIM_UNKNOWN_     // CPIN: unknown

_CONNECTION_2G_   // WS46, RFSTS
_CONNECTION_3G_
_CONNECTION_4G_
_CONNECTION_UNKNOWN_
```

### Socket state

```cpp
_SOCKET_CLOSED_   // SS: socket closed
```

### HTTP method (SSEND)

```cpp
_HTTP_POST_
_HTTP_GET_
_HTTP_RESPONSE_
```

### AT command type

```cpp
_AT_SET_    // pass to SET functions (CREG, WS46, SIMDET…)
_AT_GET_    // pass to GET functions
```

### WebSocket opcodes

```cpp
_WS_OPCODE_CONT_    // 0 — continuation frame
_WS_OPCODE_TEXT_    // 1 — text frame
_WS_OPCODE_BINARY_  // 2 — binary frame
_WS_OPCODE_CLOSE_   // 8 — close frame
_WS_OPCODE_PING_    // 9 — ping control frame
_WS_OPCODE_PONG_    // 10 — pong control frame
```

### Modem identification

```cpp
_MODEM_MANUFACTURER_TELIT_
_MODEM_MANUFACTURER_UNKNOWN_

_MODEM_MODEL_LE910C1_EUX_
_MODEM_MODEL_GE910_QUAD_
_MODEM_MODEL_GE910_QUAD_V3_
_MODEM_MODEL_GE910_GNSS_
_MODEM_MODEL_LE910S1_EA_
_MODEM_MODEL_LE910S1_EAG_
_MODEM_MODEL_LE910R1_EU_
_MODEM_MODEL_UNKNOWN_
```

---

## Configuration

All overrides must appear **before** `#include <AT_Command.h>`.

### IO Buffer

```cpp
#define _IO_Buffer_Size 256    // default 1024
```

### Timeouts (ms)

```cpp
#define _TIMEOUT_SGACT_   60000   // PDP activation (default 60 s)
#define _TIMEOUT_WSOPEN_  15000   // WebSocket handshake (default 15 s)
#define _TIMEOUT_WSRECV_  5000    // WebSocket receive (default 5 s)
#define _TIMEOUT_SSLD_    65000   // SSL dial (default 65 s)
#define _TIMEOUT_SD_      65000   // TCP dial (default 65 s)
// ... see Config.h for full list
```

### Server macros (SSEND)

```cpp
#define _PostMan_Server_    "your.server.ip"
#define _PostMan_EndPoint_  "/api/data"
#define _PostMan_Port_      80
#define _PostMan_CID_       1
```

---

## Examples

| Example | Description |
|---------|-------------|
| `01_Modem_Init` | Basic modem startup: AT, ATE, CMEE, FCLASS, CGMI, CGMM, SWPKGV, CGSN, CPIN |
| `02_Network_Connect` | SIM check, registration, APN setup, IP activation |
| `03_HTTP_Post` | TCP socket + HTTP POST with SCFG, SSEND, SRING, SRECV |
| `04_Signal_Time` | Diagnostics: CSQ, RFSTS, CTZU, CCLK, Ping |
| `05_WebSocket` | Full ws:// and wss:// connect/send/receive/ping-pong/close |

---

[![Support me](https://img.shields.io/badge/Support-PATREON-GREEN.svg)](https://www.patreon.com/bePatron?u=62967889) ![Twitter Follow](https://img.shields.io/twitter/follow/gunceakkoyun?style=social) [![E-Mail](https://img.shields.io/badge/E_Mail-Mehmet_Gunce_Akkoyun-blue.svg)](mailto:akkoyun@me.com)
