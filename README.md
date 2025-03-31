# POP3-Proxy-Server

## Overview
POP3 Proxy Server is a multi-threaded application that intermediates communication between an email client and a POP3 server. It listens for connection requests on a predefined TCP port and forwards them to the POP3 server on port 110. The proxy also supports user blocking based on email addresses provided in the `USER` command.

## Features
- 📡 **Transparent forwarding** – Relays requests from the client to the server and responses from the server to the client.
- 🔒 **User blocking** – Blocks access for specific users based on their email addresses.
- ⚡ **Multi-threaded processing** – Each connection is handled in a separate thread for efficient performance.
- ✅ **Tested Compatibility** – Works with standard POP3 clients and servers.

## Installation & Usage

### 1. Build the Project
```sh
make client_clean
make client
make proxy_clean
make proxy
```

### 2. Run the Proxy and Client
```sh
./proxy
./client
```

### 3. Start the POP3 Server (BabyPOP3Server)
The POP3 server used for testing was started on a separate computer. Ensure that the proxy is configured to connect to the correct server address.

## Configuration
- The proxy listens for incoming connections on a predefined TCP port.
- Forwarding is done to the POP3 server running on port **110**.
- User blocking is managed via a list of restricted email addresses.

## License
This open-source project is available under the [MIT License](LICENSE).

## Contributing
Pull requests are welcome! If you happen to have any issues, please feel free to open an [issue](https://github.com/yourusername/POP3-Proxy-Server/issues).

## Contact
For questions or suggestions, contact [mail](mailto:marijanink@gmail.com).
