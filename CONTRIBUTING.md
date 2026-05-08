# Contributing to AT_Command Library

Thank you for considering contributing to the AT_Command Library.

## Ways to Contribute

- **Bug reports** — open a GitHub issue with reproduction steps, expected vs actual behaviour.
- **Feature requests** — open a GitHub issue describing the use case and benefit.
- **Pull requests** — fork the repo, make your changes on a new branch, and open a PR against `main`.
- **Documentation** — typo fixes, clarifications, and new examples are always welcome.

## Pull Request Guidelines

1. Branch from `main` and keep changes focused.
2. Match the existing code style (`constexpr` constants, `&&`/`||` operators, no VLAs, no unsafe C string functions).
3. Update `CHANGELOG.md` under `[Unreleased]` with a brief description of the change.
4. If you add a new AT command function, add a corresponding entry to `keywords.txt`.

## Reporting Bugs

Please include:
- Board and MCU (e.g. Arduino Mega, ATmega2560)
- Modem model and firmware version
- Minimal code that reproduces the issue
- Observed vs expected output
