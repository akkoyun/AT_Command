# Changelog

All notable changes to this project will be documented in this file. The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/), and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [Unreleased]

### Planned

- ...

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
