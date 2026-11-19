# Keychron B1 Pro: Fn as a dictation key

A fork of [Keychron's ZMK firmware](https://github.com/Keychron/zmk/tree/keychron_bpro) for the Keychron B1 Pro (US ANSI, PID `0x071a`, based on v1.0.2) that turns the Fn key into a voice dictation key for apps like [Wispr Flow](https://wisprflow.ai), without losing any Fn shortcuts.

macOS only honours the Apple fn key from keyboards that report Apple's USB vendor ID, so a third-party keyboard's Fn never reaches the Mac. This firmware gives the Fn key three jobs in Mac mode instead:

| You do | Keyboard sends |
|---|---|
| Tap Fn (under 200 ms, no other key) | `F13`, for hands-free toggle |
| Hold Fn alone | `F16`, held until release, for push-to-talk |
| Hold Fn and press another key | The normal Fn layer (`F1`–`F12`, Home/End, pairing, and so on) |

If you hold Fn long enough to start push-to-talk and then press another key, the keyboard taps `Esc` to cancel the dictation, releases `F16`, and sends your key through the Fn layer. Fn+`F11` always produces a real `F11`.

Windows mode is unchanged.

## Install

Keychron Launcher only flashes Keychron's own firmware, so this goes through the keyboard's UF2 bootloader instead.

1. Download `keychron_b1_pro_us_fn_ptt.uf2` from the [latest release](../../releases/latest).
2. Connect the keyboard to your Mac by USB cable.
3. Switch it to Windows mode: hold **Fn + X + L** for 3 seconds.
4. Enter the bootloader: hold **Fn + –** (minus) for 3 seconds. The keyboard stops typing and a drive called `NRF52BOOT` appears.
5. Copy the `.uf2` file onto `NRF52BOOT`. The keyboard flashes it and restarts. macOS may complain that the copy failed because the drive disappeared mid-copy. That's normal.
6. Switch back to Mac mode: hold **Fn + X + L** for 3 seconds.
7. In Wispr Flow, set hands-free to `F13` and push-to-talk to `F16`.

To go back to stock, flash `stock_keychron_b1_pro_us_v1.0.2.uf2` from the same release the same way.

The release also has a `.zip` in Nordic's DFU package format, the same format Keychron ships, for flashing over the bootloader's serial port with `adafruit-nrfutil`.

## Changing it

The settings live in the `fn_ptt` node in [`keychron_b1_usn.keymap`](app/boards/shields/keychron/b1n/us/keychron_b1_usn.keymap): `tap-key`, `hold-key`, `cancel-key` (set to `0` to disable) and `tapping-term-ms`. The behaviour itself is [`behavior_fn_ptt.c`](app/src/behaviors/behavior_fn_ptt.c). Every push builds the `.uf2` and `.zip` in GitHub Actions.

---

# Zephyr™ Mechanical Keyboard (ZMK) Firmware

[![Discord](https://img.shields.io/discord/719497620560543766)](https://zmk.dev/community/discord/invite)
[![Build](https://github.com/zmkfirmware/zmk/workflows/Build/badge.svg)](https://github.com/zmkfirmware/zmk/actions)
[![Contributor Covenant](https://img.shields.io/badge/Contributor%20Covenant-v2.0%20adopted-ff69b4.svg)](CODE_OF_CONDUCT.md)

[ZMK Firmware](https://zmk.dev/) is an open source ([MIT](LICENSE)) keyboard firmware built on the [Zephyr™ Project](https://www.zephyrproject.org/) Real Time Operating System (RTOS). ZMK's goal is to provide a modern, wireless, and powerful firmware free of licensing issues.

Check out the website to learn more: https://zmk.dev/.

You can also come join our [ZMK Discord Server](https://zmk.dev/community/discord/invite).

To review features, check out the [feature overview](https://zmk.dev/docs/). ZMK is under active development, and new features are listed with the [enhancement label](https://github.com/zmkfirmware/zmk/issues?q=is%3Aissue+is%3Aopen+label%3Aenhancement) in GitHub. Please feel free to add 👍 to the issue description of any requests to upvote the feature.

[Keychron](https://keychron.com/) use zmk source code for Bpro series keyboards ,have made big changes fro Bpro, also add proprietary 2.4g communication.

To build the firmware ,for example: keychorn b1 pro

prepare:
```    
    mkdir keychron
    cd keychron
    git clone -b keychron_bpro https://github.com/keychron/zmk.git 
    cd zmk
    west init -l app/
    west update
```
patch zephyr:
```
    cd zephyr
    git am ../001-esb-nrf-fix.patch
```
build firmware:
```
    cd app
    west build -b keychron -p -- -DSHIELD=keychron_b1_us
```