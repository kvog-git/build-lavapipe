Automatic build of [Mesa's software implementation of Vulkan](https://docs.mesa3d.org/drivers/llvmpipe.html) as a standalone DLL for Windows (x64 and arm64).

This is useful for running Vulkan applications in environments without GPU access, such as VMs or CI runners.

## Download

Download the latest binaries [here](https://github.com/kvog-git/build-lavapipe/releases/latest).

## Loading with `VK_DRIVER_FILES`

Users can force specific applications to use Lavapipe by setting the `VK_DRIVER_FILES` environment variable to the path of a custom driver manifest file, e.g. [`lvp_icd.x86_64.json`](/examples/lvp_icd.x86_64.json). You can copy the manifest files from the `examples` directory (for relative paths) or [write your own](https://github.com/KhronosGroup/Vulkan-Loader/blob/main/docs/LoaderDriverInterface.md#driver-manifest-file-format).

Setting `VK_DRIVER_FILES` programmatically also works well. See [`examples/example.c`](/examples/example.c).

## Loading with `VK_LUNARG_direct_driver_loading`

Application developers can explicitly load the Lavapipe DLL using the [`VK_LUNARG_direct_driver_loading`](https://docs.vulkan.org/features/latest/features/proposals/VK_LUNARG_direct_driver_loading.html) extension. This method works very well if you are shipping the Lavapipe DLL with your app.

See [`examples/example_direct_driver_loading.c`](/examples/example_direct_driver_loading.c).

## Security

Release artifacts are cryptographically signed and can be verified with [`gh attestation verify`](https://cli.github.com/manual/gh_attestation_verify):

```sh
$ gh attestation verify --owner kvog-git ./vulkan_lvp_arm64.dll 
Loaded digest sha256:db1341d2d86ad4b80f9300cab0e8f7a78c6be7002ced094a325c7424d00d7a95 for file://vulkan_lvp_arm64.dll
Loaded 1 attestation from GitHub API

The following policy criteria will be enforced:
- Predicate type must match:................ https://slsa.dev/provenance/v1
- Source Repository Owner URI must match:... https://github.com/kvog-git
- Subject Alternative Name must match regex: (?i)^https://github.com/kvog-git/
- OIDC Issuer must match:................... https://token.actions.githubusercontent.com

✓ Verification succeeded!

The following 1 attestation matched the policy criteria

- Attestation #1
  - Build repo:..... kvog-git/build-lavapipe
  - Build workflow:. .github/workflows/build-lavapipe.yml@refs/tags/mesa-26.1.2+build1
  - Signer repo:.... kvog-git/build-lavapipe
  - Signer workflow: .github/workflows/build-lavapipe.yml@refs/tags/mesa-26.1.2+build1
  ```

## Kudos

Parts of the compile workflow were based on [mmozeiko/build-mesa](https://github.com/mmozeiko/build-mesa).

