# Changelog

All notable changes to this project will be documented in this file. The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/), and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [Unreleased]

### Planned

- ...

---

## [02.02.01] - 2026-05-09

### Fixed

- `FTPRECV`: IO buffer size was hardcoded to 1024 bytes regardless of the `_Size` parameter passed to `AT#FTPRECV`. This caused `memcpy` to copy up to 1022 bytes into the caller's buffer even when the caller requested fewer bytes, leading to a buffer overflow for small `_Data` allocations. Buffer size is now computed as `_Size + 30` (data + header/footer overhead) capped at `_IO_Buffer_Size`.

---

## [02.02.00] - 2026-05-09

### Added

- WSS (`wss://`) support: all 6 WebSocket functions (`WSOPEN`, `WSSEND`, `WSRECV`, `WSPING`, `WSPONG`, `WSCLOSE`) accept a new `_SSL = false` default parameter. When `true`, the functions use `AT#SSLD` / `AT#SSLSEND` / `AT#SSLRECV` / `AT#SSLH` instead of their plain-TCP equivalents. SSL is configured with cipher-suite auto-negotiation and no certificate verification (`authMode=0`) suitable for IoT devices.
- Extended WebSocket payload length: `WSSEND` now supports payloads up to 65535 bytes by encoding the 16-bit extended length field (`0xFE` + 2-byte big-endian) when payload > 125 bytes. `WSRECV` parses both 7-bit and 16-bit server-frame length fields.
- SSL timeout constants in `Config.h`: `_TIMEOUT_SSLCFG_` (1 s), `_TIMEOUT_SSLSECCFG_` (1 s), `_TIMEOUT_SSLD_` (65 s), `_TIMEOUT_SSLH_` (5 s).

### Changed

- `WSSEND`: removed the 125-byte payload limit (was an artificial restriction).

---

## [02.01.01] - 2026-05-09

### Fixed

- `WSOPEN` step 3: replaced single `AT#SRECV` call with a retry-poll loop. The Telit modem returns `ERROR` immediately when the receive buffer is empty, so polling every 500 ms (up to `_TIMEOUT_WSOPEN_`) is required to correctly wait for the server's `101 Switching Protocols` response.

### Added

- `WSPONG(connID)` — sends a masked pong control frame (RFC 6455 §5.5.3). Required when `WSRECV` returns `opcode == _WS_OPCODE_PING_`; the RFC mandates a pong reply or the server will close the connection.
- Example `05_WebSocket` — full connect/send/receive/ping-pong/close demo with server-ping handling.

---

## [02.01.00] - 2026-05-08

### Added

- WebSocket support (RFC 6455) over the existing TCP socket infrastructure:
  - `WSOPEN(connID, host, port, path)` — opens a TCP connection then performs the HTTP/1.1 Upgrade handshake; verifies `101 Switching Protocols` response.
  - `WSSEND(connID, data)` — sends a masked text frame; masking key is selected such that no wire byte equals `0x1A`, preventing premature `AT#SSEND` termination.
  - `WSRECV(connID, data, maxLen, opcode)` — reads one server frame using a binary-safe buffer read, parses the RFC 6455 header, and returns the unmasked payload.
  - `WSPING(connID)` — sends a masked ping control frame with an empty payload.
  - `WSCLOSE(connID)` — sends a masked close frame then closes the underlying TCP socket via `SH`.
- `Read_UART_Buffer_Raw` private helper — same as `Read_UART_Buffer` but advances `Read_Order` for every byte regardless of ASCII value, enabling correct handling of binary WebSocket frame headers.
- WebSocket opcode constants in `Definations.h`: `_WS_OPCODE_CONT_`, `_WS_OPCODE_TEXT_`, `_WS_OPCODE_BINARY_`, `_WS_OPCODE_CLOSE_`, `_WS_OPCODE_PING_`, `_WS_OPCODE_PONG_`.
- WebSocket timeout constants in `Config.h`: `_TIMEOUT_WSOPEN_` (15 s), `_TIMEOUT_WSSEND_` (2 s), `_TIMEOUT_WSRECV_` (5 s).

---

## [02.00.01] - 2026-05-08

This release is a full refactor of the library. No new AT commands were added; all changes focus on correctness, memory safety, and code quality.

### Memory

- Shared IO buffer (`_IO_Buffer[1024]`) added as a class member — replaces ~60 per-function stack allocations, eliminating stack overflow risk on AVR targets.
- Variable Length Arrays (VLA) completely eliminated across all functions.

### Type Safety

- All typed `#define` constants in `Definations.h` converted to `constexpr` (79 constants).
- All numeric `#define` constants in `Config.h` converted to `constexpr` with `#ifndef` override guards.
- String literal constants (`_PostMan_Server_`, `_PostMan_EndPoint_`) kept as `#define` for user override compatibility.
- `#pragma once` replaces old-style `#ifndef` include guards in all headers.
- `atoi` replaced with `strtoul(..., 10)` — avoids signed/unsigned conversion hazard.
- `strtol` replaced with `strtoul(..., 16)` in `Handle_HEX` — consistent unsigned parsing.

### Bug Fixes

- `Read_UART_Buffer`: off-by-one fixed — `Read_Order` no longer advances after response detection, preserving correct parse positions in position-sensitive functions (CEER, CREG, SS, etc.).
- `Find_Char`: returns `UINT16_MAX` as sentinel instead of `0` when character is not found, eliminating ambiguity with a legitimate position-0 match.
- `Handle_Number` / `Handle_HEX`: guard updated to check `UINT16_MAX` sentinel before computing size.
- `CGMM`: model string detection order fixed — `GE910-QUAD-V3` is now checked before `GE910-QUAD` to prevent false substring match.
- `CCLK`: negative timezone offset now correctly parsed via `sscanf` with sign character capture; previously caused underflow in `Handle_Number`.
- `SCFGEXT3`: AT command corrected from `AT#SCFGEXT2=` to `AT#SCFGEXT3=`.
- `DNS`: missing comma separator in AT command string fixed.
- `RFSTS` 2G/3G branches: now clear output variables and return `false` instead of leaving outputs undefined.
- `SS`: `isdigit()` validation added for socket state character before conversion.
- `CEER`: `isdigit()` guards added for all error-code digit positions.
- `sscanf` return value checks added to: `CGMI`, `CGMM`, `CPIN`, `CREG`, `WS46`, `FTPFSIZE`.

### Style

- `and` / `or` keyword operators replaced with `&&` / `||` throughout all code paths.
- `Find()`: raw ASCII integer literals (`13`, `10`, `79`, `75`, …) replaced with character literals (`'\r'`, `'\n'`, `'O'`, `'K'`, …).
- Magic delay values extracted to named constants: `_AT_UART_READ_DELAY_` (5 ms), `_AT_SD_PROMPT_DELAY_` (10 ms).
- CSQ→RSSI mapping annotated with 3GPP TS 27.007 §8.5 reference.
- `_Buffer[_Size - 0]` simplified to `_Buffer[_Size]` in `Find()`.
- Typos corrected: "Nanual DNS" → "Manual DNS", "Detech" → "Detach".

### Examples

- `01_Modem_Init` — basic modem startup: AT, ATE, CMEE, FCLASS, CGMI, CGMM, SWPKGV, CGSN, CPIN.
- `02_Network_Connect` — SIM check, network registration, APN config, IP activation: CPIN, CCID, CREG, CGDCONT, SGACT.
- `03_HTTP_Post` — TCP socket connection and HTTP POST: SCFG, SCFGEXT, ATSD, SSEND, SRING, SRECV, SH.
- `04_Signal_Time` — diagnostics: CSQ, RFSTS, CTZU, CCLK, Ping.

---

## [1.9.1]

- Ping function updated.
- Bug fixes.

## [1.9.0]

- Ping function updated.
- DNS function added.
- CACHEDNS function added.

## [1.8.3]

- CID variable is updated.

## [1.8.2]

- Callback_JSON_Parse function is updated.
- SSEND function is updated.

## [1.8.1]

- Definitions updated.
- Handle_Number function moved to private functions.
- Find_Char function moved to private functions.
- FTPCWD, FTPGETPKT, FTPTO and FTPTYPE function temp buffer size is updated from to 30.

## [1.8.0]

- FTPFSIZE function updated.
- FTPGETPKT function updated.
- FTPRECV function updated.

## [1.7.0]

- Parse_Number function name changed to Handle_Number.
- RFSTS function updated. TAC and CellID variables added.
- RFSTS connection type attribute added.

## [1.6.0]

- SKTTO function added.
- `_TIMEOUT_SKTTO_` constant added.

## [1.5.0]

- All AT functions moved to protected scope.
- SRING function timeout changed from 50 s to 1 s.

## [1.4.0]

- TCPMAXWIN function added.
