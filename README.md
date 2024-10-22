# CommonLibSSE-NG

Fork of a fork of a fork of a fork of a...

## Usage

Use the [plugin template](https://github.com/ThirdEyeSqueegee/CLibNGPluginTemplate) for the quickest start. For manual setup:

Add the following to your project's `vcpkg.json`:

**NOTE**: Full cross-runtime support (SE, AE, and VR) is enabled by default

```json
"dependencies": ["commonlibsse-ng", "..."],
// or, to enable xbyak support for the trampoline:
// "dependencies": [{ "name": "commonlibsse-ng", "features": ["xbyak"] }, ...]
// or, to only build for flatrim (no VR)
// "dependencies": [{ "name": "commonlibsse-ng", "default-features": false, "features": ["flatrim"] }, ...]
...
"vcpkg-configuration": {
  "registries": [
    {
      "kind": "git",
      "repository": "https://github.com/ThirdEyeSqueegee/CommonLibSSE-NG-vcpkg",
      "baseline": "<hash of the latest commit in the above repo>",
      "packages": ["commonlibsse-ng"]
    },
    {
      "kind": "git",
      "repository": "https://github.com/ThirdEyeSqueegee/vcpkg-ports",
      "baseline": "<hash of the latest commit in the above repo>",
      "packages": ["openvr", "spdlog"]
    }
  ],
}
```

Add the following to your project's `CMakeLists.txt`:

```cmake
find_package(CommonLibSSE CONFIG REQUIRED)

add_commonlibsse_plugin(
  ${PROJECT_NAME}
  AUTHOR <your name>
  SOURCES <your source files>
)
```

## [Differences from upstream](https://github.com/ThirdEyeSqueegee/CommonLibSSE-NG/compare/ng...main)

- Only `all` (SE, AE, VR -- default) and `flatrim` (SE, AE) presets
- Less cruft (no Conan, no Xmake, no tests, no `clang-cl` support)
- `spdlog` uses `std::format`; no implicit `fmt` dependency
- Available as a vcpkg port (see [Usage](#usage))

## Comparisons

- [powerof3/CommonLibSSE/dev](https://github.com/ThirdEyeSqueegee/CommonLibSSE-NG/compare/main...powerof3:CommonLibSSE:dev)
- [alandtse/CommonLibVR/ng](https://github.com/ThirdEyeSqueegee/CommonLibSSE-NG/compare/main...ng)

## History

[Ryan-rsm-McKenzie/CommonLibSSE](https://github.com/Ryan-rsm-McKenzie/CommonLibSSE) -> [powerof3/CommonLibSSE](https://github.com/powerof3/CommonLibSSE) -> [CharmedBaryon/CommonLibSSE-NG](https://github.com/CharmedBaryon/CommonLibSSE-NG) -> [alandtse/CommonLibVR/ng](https://github.com/alandtse/CommonLibVR/tree/ng) -> [this repo](#)
