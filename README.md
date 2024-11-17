# Secure Encryptor
A command-line tool for encrypting and decrypting files using the XChaCha20 algorithm.

### Supported platforms
At the moment it supports macOS only

### How to encrypt the file
```bash
./bin/senc ~/Desktop/input.file ~/Desktop/encrypted.file
```

### How to decrypt the file
```bash
./bin/sdec ~/Desktop/encrypted.file ~/Desktop/decrypted.file
```

### How to compile the project
```bash
git clone https://github.com/iharkatkavets/secure-encryptor.git && cd secure-encryptor
make all
# Binary files located in bin dir
ls ./bin
```

### Encrypted file format
XChaCha20 operates with a 256-bit key and a 192-bit nonce. It is safe to place the nonce at the beginning of the encrypted file. The hash of the key(password) is located after the padded bytes. 
All content except nonce is encrypted using XChaCha20 algorithm.
```text
      24 bytes             2 bytes            PAD_SIZE bytes           32 bytes       
[NON_ENCRYPTED_NONCE][ENCRYPTED_PAD_SIZE][ENCRYPTED_RANDOM_BYTES][ENCRYPTED_KEY_HASH][ENCRYPTED_FILE]
```
**NON_ENCRYPTED_NONCE (24 bytes):**<br>
•	Stores the 192-bit nonce.<br>
•	The nonce is uniquely generated for every encryption operation using the same key. It is non-sensitive and can be safely stored without encryption.

**ENCRYPTED_PAD_SIZE (2 bytes):**<br>
•	Contains the size of random padding bytes in the file, encrypted to hide potential file structure hints.<br>
•	2 bytes (16 bits) allow for a maximum pad size of 65,535 bytes, which should be sufficient.

**ENCRYPTED_RANDOM_BYTES (PAD_SIZE bytes):**<br>
•	Random padding added to prevent file size inference attacks.<br>
•	These bytes are encrypted to prevent exposing the padding length directly.

**ENCRYPTED_KEY_HASH (32 bytes):**<br>
•	A 256-bit hash of the encryption key, encrypted with the key itself.<br>
•	This allows verification that the correct key is being used during decryption.

**ENCRYPTED_FILE (FILE_SIZE bytes):**<br>
•	The encrypted main payload.

# Inspirations and Contributions
This project draws inspiration from the video lectures of [Dr. Jonas Birch](https://www.linkedin.com/in/jonasbirch/). The concept for the encrypted file format adapts ideas presented in those lectures.

Additionally, sources from the repository [vitorstraggiotti/easySHA256](https://github.com/vitorstraggiotti/easySHA256) were utilized to implement SHA256.

And sources from the repository [spcnvdr/xchacha20](https://github.com/spcnvdr/xchacha20) were utilized to implement XChaCha20.

# License
```
MIT License

Copyright (c) 2024 Ihar Katkavets

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```
This project also includes code from [spcnvdr/xchacha20](https://github.com/spcnvdr/xchacha20) by Bryan Hawkins, licensed under the BSD-3-Clause License:
```
Copyright 2019 Bryan Hawkins <spcnvdrr@protonmail.com>

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1. Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its
contributors may be used to endorse or promote products derived from
this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
```
