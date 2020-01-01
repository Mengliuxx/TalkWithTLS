# TalkWithTLS
Sample Code for TLS usage and a lot more than that.
- It has sample code to use OpenSSL and wolfSSL.
- And it has a python based automated test framework using Pytest to test OpenSSL
- Finally a perf script to test various TLS implementations.

## 1. Installing Dependencies
```
sudo apt install make gcc python python-pip
pip install --user pytest pytest-html
```

## 2. Building
```
make
```

### 2.1 Building only specific binaries
- `make sample_bin` To build only Sample binaries
- `make test_bin` To build only Test binaries
- `make perf_bin` To build only Performance script binaries
