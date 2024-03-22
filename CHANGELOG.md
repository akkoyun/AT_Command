# Changelog

All notable changes to this project will be documented in this file. The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/), and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [Unreleased]

### Planned

- ...

---

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

- Parse_Number function name changed to Handle_Number
- RFSTS function updated. TAC and CellID variables added. (TAC and CellID are used for location information.)
- RFSTS connection type attribute added. (Connection type is get from WS46 function.)

## [1.6.0]

- SKTTO Function added. (Socket timeout function.)
- _TIMEOUT_SKTTO_ constant added. (Socket timeout constant.)

## [1.5.0]

- All AT functions are moved to protected functions. With these change the library is more secure and stable.
- SRING function (that just looks for SRING string) time out is changed form 50sn to 1sn.

## [1.4.0]

- TCPMAXWIN Function added. (Firmware download CMEE error fix function.)
