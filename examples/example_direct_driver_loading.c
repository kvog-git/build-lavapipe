// ================================================================================================
// This is a basic demonstration showing how to load the Lavapipe Vulkan software implementation
// by using the VK_LUNARG_direct_driver_loading extension.
//
// ref: https://docs.vulkan.org
// ref: https://github.com/kvog-git/demos/blob/master/vk-cube/vk-cube.c
//
// Build:
//     > cl /W4 /Od /Zi example_direct_driver_loading.c /Fe:example_direct_driver_loading.exe /I "%VULKAN_SDK%\Include" /link /LIBPATH:"%VULKAN_SDK%\Lib"
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
    
    // Require the VK_LUNARG_direct_driver_loading instance extension
    const char *instance_extensions[] = { "VK_LUNARG_direct_driver_loading" };
    create_info.enabledExtensionCount   = 1;
    create_info.ppEnabledExtensionNames = instance_extensions;

    // Manually load the Lavapipe Vulkan DLL
    #ifdef _M_X64
    HMODULE lvp = LoadLibraryA("vulkan_lvp_x64.dll");
    #endif
    #ifdef _M_ARM64
    HMODULE lvp = LoadLibraryA("vulkan_lvp_arm64.dll");
    #endif

    if (!lvp) {
        printf("Failed to load Lavapipe Vulkan DLL: (GetLastError() = %d)\n", GetLastError());
        return EXIT_FAILURE;
    }

    FARPROC lvp_factory = GetProcAddress(lvp, "vk_icdGetInstanceProcAddr");
    if (!lvp_factory) {
        printf("Failed to find vk_icdGetInstanceProcAddr in Lavapipe Vulkan DLL\n");
        return EXIT_FAILURE;
    }

    VkDirectDriverLoadingInfoLUNARG lvp_loader = {
        .sType                  = VK_STRUCTURE_TYPE_DIRECT_DRIVER_LOADING_INFO_LUNARG,
        .pfnGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)lvp_factory
    };
    VkDirectDriverLoadingListLUNARG direct_driver_loading = {
        .sType       = VK_STRUCTURE_TYPE_DIRECT_DRIVER_LOADING_LIST_LUNARG,
        .mode        = VK_DIRECT_DRIVER_LOADING_MODE_EXCLUSIVE_LUNARG,
        .driverCount = 1,
        .pDrivers    = &lvp_loader,
    };

    // Point to loader list
    create_info.pNext = &direct_driver_loading;
    
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
