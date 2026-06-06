@echo off

if not exist vulkan_lvp_arm64.dll (
    curl -LO --fail https://github.com/kvog-git/build-lavapipe/releases/latest/download/vulkan_lvp_arm64.dll
)
if not exist vulkan_lvp_x64.dll (
    curl -LO --fail https://github.com/kvog-git/build-lavapipe/releases/latest/download/vulkan_lvp_x64.dll
)

cl /W4 /Od /Zi example.c                       /Fe:example.exe                       /I "%VULKAN_SDK%\Include" /link /LIBPATH:"%VULKAN_SDK%\Lib"
cl /W4 /Od /Zi example_direct_driver_loading.c /Fe:example_direct_driver_loading.exe /I "%VULKAN_SDK%\Include" /link /LIBPATH:"%VULKAN_SDK%\Lib"