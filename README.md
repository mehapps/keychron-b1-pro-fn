# Keychron B1 Pro: Fn as a dictation and Globe key

A fork of [Keychron's ZMK firmware](https://github.com/Keychron/zmk/tree/keychron_bpro) for the Keychron B1 Pro (US ANSI, PID `0x071a`, based on v1.0.2) that makes the Fn key work more like a Mac's, without losing any of Keychron's Fn shortcuts.

macOS only honours the Apple fn key from keyboards that report Apple's USB vendor ID, so a third-party keyboard's Fn never reaches the Mac. This firmware adds two independent features in Mac mode instead.

**Dictation key**, for apps like [Wispr Flow](https://wisprflow.ai):

| You do | Keyboard sends |
|---|---|
| Tap Fn (under 200 ms, no other key) | `F13`, for hands-free toggle |
| Hold Fn alone | `F16`, held until release, for push-to-talk |
| Hold Fn and press another key | The Fn layer |

If you hold Fn long enough to start push-to-talk and then press another key, the keyboard taps `Esc` to cancel the dictation, releases `F16`, and sends your key through the Fn layer.

**Globe shortcuts:** Fn + a letter sends Globe + that letter, so macOS's Globe shortcuts work: Fn+N for Notification Center, Fn+C for Control Center, Fn+F for full screen, Fn+E for emoji, Fn+Q for Quick Note, and so on. Keys that already have an Fn job keep it: `F1`–`F12` stay real F-keys, `1`–`4` pair Bluetooth, `B` shows the battery, `I` is Insert, and the arrows are Home/End/PgUp/PgDn.

Every build also fixes Ctrl+Shift+Tab, which Keychron's public source silently drops (its anti-ghosting exception is never compiled in), and enables Keychron's adaptive NKRO to match the stock firmware. Windows mode is unchanged.

## Builds

Each release has four builds. Pick the features you want:

| File | Dictation key | Globe shortcuts |
|---|---|---|
| `keychron_b1_pro_us_both.uf2` | ✓ | ✓ |
| `keychron_b1_pro_us_wispr.uf2` | ✓ | |
| `keychron_b1_pro_us_globe.uf2` | | ✓ |
| `keychron_b1_pro_us_none.uf2` | | |

`none` behaves like Keychron's stock firmware, plus the Ctrl+Shift+Tab fix.

## Install

Keychron Launcher only flashes Keychron's own firmware, so this goes through the keyboard's UF2 bootloader instead.

1. Download the `.uf2` you want from the [latest release](../../releases/latest).
2. Connect the keyboard to your Mac by USB cable.
3. Switch it to Windows mode: hold **Fn + X + L** for 3 seconds.
4. Enter the bootloader: hold **Fn + –** (minus) for 3 seconds. The keyboard stops typing and a drive called `NRF52BOOT` appears.
5. Copy the `.uf2` file onto `NRF52BOOT`. The keyboard flashes it and restarts. macOS may complain that the copy failed because the drive disappeared mid-copy. That's normal.
6. Switch back to Mac mode: hold **Fn + X + L** for 3 seconds.
7. With a dictation build, set Wispr Flow's hands-free shortcut to `F13` and push-to-talk to `F16`.

To go back to stock, flash `stock_keychron_b1_pro_us_v1.0.2.uf2` from the same release the same way.

Each build also comes as a `.zip` in Nordic's DFU package format, the same format Keychron ships, for flashing over the bootloader's serial port with `adafruit-nrfutil`.

## Changing it

Everything lives in [`keychron_b1_usn.keymap`](app/boards/shields/keychron/b1n/us/keychron_b1_usn.keymap):

- **Dictation key:** the `fn_ptt` node sets `tap-key`, `hold-key`, `cancel-key` (`0` to disable) and `tapping-term-ms`. The behaviour itself is [`behavior_fn_ptt.c`](app/src/behaviors/behavior_fn_ptt.c).
- **Globe shortcuts:** the `gkp` macro, used as `GLB(key)` in the Fn layer.
- **Features:** building with `-DDTS_EXTRA_CPPFLAGS="-DNO_FN_PTT"` or `"-DNO_FN_GLOBE"` turns a feature off. Separate both with `;`.

Every push builds all four variants in GitHub Actions.

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