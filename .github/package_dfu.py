import json
import struct
import sys
import zipfile


def read_hex(path):
    mem = {}
    base = 0
    for line in open(path):
        line = line.strip()
        if not line.startswith(":"):
            continue
        raw = bytes.fromhex(line[1:])
        count, addr, kind = raw[0], int.from_bytes(raw[1:3], "big"), raw[3]
        data = raw[4 : 4 + count]
        if kind == 0:
            for i, b in enumerate(data):
                mem[base + addr + i] = b
        elif kind == 2:
            base = int.from_bytes(data, "big") << 4
        elif kind == 4:
            base = int.from_bytes(data, "big") << 16
    lo, hi = min(mem), max(mem) + 1
    if hi - lo > 0x100000:
        sys.exit(f"hex spans {hi - lo} bytes, expected a single application image")
    return lo, bytes(mem.get(a, 0xFF) for a in range(lo, hi))


def write_uf2(base, app, out, family=0x239A0029):
    blocks = (len(app) + 255) // 256
    with open(out, "wb") as f:
        for i in range(blocks):
            data = app[i * 256 : (i + 1) * 256].ljust(476, b"\x00")
            header = struct.pack(
                "<8I", 0x0A324655, 0x9E5D5157, 0x2000, base + i * 256, 256, i, blocks, family
            )
            f.write(header + data + struct.pack("<I", 0x0AB16F30))


def crc16(data):
    crc = 0xFFFF
    for x in data:
        crc = ((crc >> 8) | (crc << 8)) & 0xFFFF
        crc ^= x
        crc ^= (crc & 0xFF) >> 4
        crc ^= (crc << 12) & 0xFFFF
        crc ^= ((crc & 0xFF) << 5) & 0xFFFF
    return crc


def package(app, out, name="zmk_usn"):
    crc = crc16(app)
    dat = struct.pack("<HHIHHH", 0x0052, 0xFFFF, 0xFFFFFFFF, 1, 0xFFFE, crc)
    manifest = {
        "manifest": {
            "application": {
                "bin_file": f"{name}.bin",
                "dat_file": f"{name}.dat",
                "init_packet_data": {
                    "application_version": 0xFFFFFFFF,
                    "device_revision": 0xFFFF,
                    "device_type": 0x0052,
                    "firmware_crc16": crc,
                    "softdevice_req": [0xFFFE],
                },
            },
            "dfu_version": 0.5,
        }
    }
    with zipfile.ZipFile(out, "w", zipfile.ZIP_DEFLATED) as z:
        z.writestr(f"{name}.bin", app)
        z.writestr(f"{name}.dat", dat)
        z.writestr("manifest.json", json.dumps(manifest, indent=4))


if __name__ == "__main__":
    base, app = read_hex(sys.argv[1])
    if base != 0x1000:
        sys.exit(f"application starts at {base:#x}, the B1 Pro bootloader expects 0x1000")
    package(app, sys.argv[2])
    write_uf2(base, app, sys.argv[3])
