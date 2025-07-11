# fcrypt

A command-line tool for encrypting files using the XChaCha20 algorithm.

## Supported platforms

At the moment it supports macOS only

## How to encrypt the file

```bash
./bin/fcrypt -e origin.file -o encrypted.file 
```

## How to decrypt the file

```bash
./bin/fcrypt -d encrypted.file
# or
./bin/fcrypt -d encrypted.file -o decrypted.file
```

## Arguments and Parameters

- **`-e <FILE>`, `--encrypt <FILE>`**  
  Encrypts the input file. The password will be prompted  
  _Example:_ `fcrypt -e origin.file -o encrypted.file`

- **`-P <SIZE>`, `--padsize <SIZE>`**  
  The pad size. Must be in range [0,65535]. Taken random size if
  not provided.  
  _Example:_ `fcrypt -e origin.file -P 16 -o encrypted.file`

- **`-p <PASSWORD>`, `--password <PASSWORD>`**  
  The password of len up 256 characters.  
  _Example:_ `fcrypt -e origin.file -k 'password' -o encrypted.file`  
  _Example:_ `fcrypt -d encrypted.file -k 'password'`

- **`-i <hint>`, `--hint <HINT>`**  
  The hint to remember the password  
  _Example:_ `fcrypt -e origin.file -i 'hint' -o encrypted.file`  
  _Example:_ `fcrypt -d encrypted.file`

- **`-o`, `--output <FILE>`**  
  The output file with encrypted content  
  _Example:_ `fcrypt -e origin.file -o encrypted.file`  
  _Example:_ `fcrypt -d encrypted.file -o origin.file`

- **`-d`, `--decrypt`**  
  Decrypts the input file. The password will be prompted  
  _Example:_ `fcrypt -d encrypted.file`

- **`-n`, `--nohint`**  
  Don't request to input hint  
  _Example:_ `fcrypt -n -e origin.file -o encrypted.file`

- **`-v`, `--verbose`**  
  Add more output  
  _Example:_ `fcrypt -v -e origin.file -o encrypted.file`

- **`-V`**  
  Print version number and exit  
  _Example:_ `fcrypt -V`

- **`-h`, `--help`**  
  Print help  
  _Example:_ `fcrypt -h`

## How to compile the project

```bash
git clone https://github.com/iharkatkavets/fcrypt.git && cd fcrypt
make fcrypt
# Binary file located in bin dir
ls ./bin
```

## Encrypted file format

XChaCha20 operates with a 256-bit key and a 192-bit nonce. It is safe to place
the nonce at the beginning of the encrypted file. The hash of the key(password)
is located after the padded bytes.
All content except HINT_LEN, PASSWORD HINT, NONCE is encrypted using XChaCha20 algorithm.

```text
                              ↓ ENCRYPTED FILE FORMAT ↓                      
+---------+----------+----------------+---------------+-----------+----------------+-----------+-----------+
| 2 bytes | 2 bytes  | HINT_LEN bytes |   24 bytes    |  2 bytes  | PAD_SIZE bytes | 32 bytes  | ...       |
+---------+----------+----------------+---------------+-----------+----------------+-----------+-----------+
| FORMAT  | HINT_LEN | NON ENCRYPTED  | NON ENCRYPTED | RANDOM    |  ENCRYPTED     | ENCRYPTED | ENCRYPTED |
| VERSION |    LE    | PASSWORD HINT  | NONCE (IV)    | ENCRYPTED |  PADDING       | KEY HASH  | SRC FILE  |   
|   LE    |          |                |               | PAD_SIZE  |  BYTES         | (SHA256)  |           |
+---------+----------+----------------+---------------+-----------+----------------+-----------+-----------+
```

**FORMAT_VERSION (2 bytes, little endian):**  
• Stores version of the format used

**HINT_LEN (2 bytes):**  
• The len of the following `NON ENCRYPTED PASSWORD HINT`

**PASSWORD_HINT (HINT_LEN bytes):**  
• Stores the password hint.  

**NON_ENCRYPTED_NONCE (24 bytes):**  
• Stores the 192-bit nonce.  
• The nonce is uniquely generated for every encryption operation using the same
key. It is non-sensitive and can be safely stored without encryption.

**ENCRYPTED_PAD_SIZE (2 bytes):**  
• Contains the size of the following padding bytes in the file, encrypted to hide
potential file structure hints.  
• 2 bytes (16 bits) allow for a maximum pad size of 65,535 bytes, which should
be sufficient.

**ENCRYPTED_RANDOM_BYTES (PAD_SIZE bytes):**  
• Random padding added to prevent file size inference attacks.  
• These bytes are encrypted to prevent exposing the padding length directly.  

**ENCRYPTED_KEY_HASH (32 bytes):**  
• A 256-bit hash of the encryption key, encrypted with the key itself.  
• This allows verification that the correct key is being used during decryption.

**ENCRYPTED_FILE (FILE_SIZE bytes):**  
• The encrypted main payload.

## Inspirations and Contributions

This project draws inspiration from the video lectures of [Dr. Jonas
Birch](https://www.linkedin.com/in/jonasbirch/). The concept for the encrypted
file format adapts ideas presented in those lectures.

Additionally, sources from the repository
[vitorstraggiotti/easySHA256](https://github.com/vitorstraggiotti/easySHA256)
were utilized to implement SHA256.

And sources from the repository
[spcnvdr/xchacha20](https://github.com/spcnvdr/xchacha20) were utilized to
implement XChaCha20.

## License

```txt
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

This project also includes code from
[spcnvdr/xchacha20](https://github.com/spcnvdr/xchacha20) by Bryan Hawkins,
licensed under the BSD-3-Clause License:

```txt
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
