// ================================================================================================
// This is a basic demonstration showing how to load the Lavapipe Vulkan software implementation
// by setting the VK_DRIVER_FILES environment variable.
//
// ref: https://docs.vulkan.org
// ref: https://github.com/kvog-git/demos/blob/master/vk-cube/vk-cube.c
//
// Build:
//     > cl /W4 /Od /Zi example.c /Fe:example.exe /I "%VULKAN_SDK%\Include" /link /LIBPATH:"%VULKAN_SDK%\Lib"
//
// License:
//     Copyright (c) 2026 Hunter Kvalevog
//
//     Permission to use, copy, modify, and/or distribute this software for any
//     purpose with or without fee is hereby granted.
//
//     THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
//     WITH REGARD TO THIS SOFTWARE.
// ================================================================================================

#include <stdio.h>
#include <stdlib.h>
#include <vulkan/vulkan.h>
#include <windows.h>

#pragma comment(lib, "vulkan-1.lib")

int main(int argc, const char **argv)
{
    (void)argc;
    (void)argv;

    VkApplicationInfo app_info = {
        .sType      = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .apiVersion = VK_API_VERSION_1_4
    };

    VkInstanceCreateInfo create_info = {
        .sType            = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &app_info
    };

    VkInstance vki = VK_NULL_HANDLE;
    VkResult   vkr = VK_SUCCESS;

    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    
    // Register a custom loader by setting the VK_DRIVER_FILES environment variable.
    #ifdef _M_X64
    _putenv_s("VK_DRIVER_FILES", "lvp_icd.x86_64.json");
    #endif
    #ifdef _M_ARM64
    _putenv_s("VK_DRIVER_FILES", "lvp_icd.aarch64.json");
    #endif
    
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    vkr = vkCreateInstance(&create_info, 0, &vki);
    if (vkr != VK_SUCCESS) {
        printf("vkCreateInstance failed: %d\n", vkr);
        return EXIT_FAILURE;
    }

    uint32_t num_devs = 0;
    vkEnumeratePhysicalDevices(vki, &num_devs, 0);

    VkPhysicalDevice *devs = calloc(num_devs, sizeof(VkPhysicalDevice));
    vkEnumeratePhysicalDevices(vki, &num_devs, devs);

    printf("Available physical devices:\n");
    for (uint32_t i = 0; i < num_devs; ++i) {
        // Get basic device properties (name)
        VkPhysicalDeviceProperties2 properties = {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2,
        };
        vkGetPhysicalDeviceProperties2(devs[i], &properties);

        printf("%d:\n", i + 1);
        printf("    name: %s:\n", properties.properties.deviceName);
        printf("    type: ");

        switch (properties.properties.deviceType) {
#define X(E) case E: printf(#E "\n"); break
        X(VK_PHYSICAL_DEVICE_TYPE_OTHER);
        X(VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU);
        X(VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
        X(VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU);
        X(VK_PHYSICAL_DEVICE_TYPE_CPU);
#undef X
        }

        // Call at least one function from the instance function table to catch ABI mismatch crashes
        VkDeviceCreateInfo device_info = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        };
        VkDevice dev;
        vkr = vkCreateDevice(devs[i], &device_info, 0, &dev);
        if (vkr != VK_SUCCESS) {
            printf("vkCreateDevice failed: %d\n", vkr);
            return EXIT_FAILURE;
        }
    }
    free(devs);
}
