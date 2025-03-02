import subprocess
import argparse

def xor_and_encode(input_file, output_file):
    key = "HB/783-/48@Nvfebv"
    key_len = len(key)
    result = bytearray()

    with open(input_file, 'rb') as f_in:
        i = 0
        while (byte := f_in.read(1)):
            result.append(byte[0] ^ ord(key[i % key_len]))
            i += 1

    encoded = subprocess.check_output(
        ['python', '-c', 'import base64; import sys; sys.stdout.buffer.write(base64.b64encode(sys.stdin.buffer.read()))'],
        input=bytes(result)
    )

    with open(output_file, 'wb') as f_out:
        f_out.write(encoded)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="XOR and base64 encode a file")
    parser.add_argument('input_file', type=str, help="Path to the input file")
    parser.add_argument('output_file', type=str, help="Path to the output file")
    args = parser.parse_args()
    xor_and_encode(args.input_file, args.output_file)

