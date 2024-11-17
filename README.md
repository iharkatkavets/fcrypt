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
•	The nonce is generated unique for every encryption operation with the same key. It’s non-sensitive and can safely be stored unencrypted.

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
