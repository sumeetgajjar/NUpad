![build and run tests](https://github.com/sumeetgajjar/NUpad/workflows/build%20and%20run%20tests/badge.svg)

# NUpad
This repository contains implementation of a Conflict Free Replicated JSON datatype.

#### Docker build

```bash
git clone https://github.com/sumeetgajjar/NUpad
cd NUpad
git submodule update --init --recursive
docker build . -t nupad
```