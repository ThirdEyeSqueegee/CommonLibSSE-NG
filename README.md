# CommonLibSSE-NG

Fork of a fork of a fork of a fork of a...

## Usage

Use the [plugin template](https://github.com/ThirdEyeSqueegee/CLibNGPluginTemplate) for the quickest start. For manual setup:

Add the following to your project's `vcpkg.json`:

```json
"dependencies": ["commonlibsse-ng", "..."],
// or
// "dependencies": [{ "name": "commonlibsse-ng", "features": ["xbyak"] }, ...]
...
"vcpkg-configuration": {
  "registries": [
    {
      "kind": "git",
      "repository": "https://github.com/ThirdEyeSqueegee/CommonLibSSE-NG-vcpkg",
      "baseline": "<hash of the latest commit in the above repo>",
      "packages": ["commonlibsse-ng", "spdlog"]
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

## History

[Ryan-rsm-McKenzie/CommonLibSSE](https://github.com/Ryan-rsm-McKenzie/CommonLibSSE) -> [powerof3/CommonLibSSE](https://github.com/powerof3/CommonLibSSE) -> [CharmedBaryon/CommonLibSSE-NG](https://github.com/CharmedBaryon/CommonLibSSE-NG) -> [alandtse/CommonLibVR/ng](https://github.com/alandtse/CommonLibVR/tree/ng) -> [this repo](#)
