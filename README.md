# skynet

Networking library for C++

# brief

This library requires C++23

This library uses CMake workflows (CMake >= 3.25.0) for all its build scripts

# install

Install location is defined in **installDir** [CMakePresets.json](./CMakePresets.json)

```sh
cmake --workflow --preset=static/shared-install
```

# benchmark

```sh
cmake --workflow --preset=static/shared-benchmark | grep -v 'SKIPPED'
```

# test

```sh
cmake --workflow --preset=static/shared-test
```
