/*
 *
 * Copyright © 2010-2011 Balázs Tóth <tbalazs@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#include "clwrapper.hpp"

CLWrapper *CLWrapper::instance = 0;

CLWrapper::CLWrapper(cl_device_type device_type) : _device_type(device_type) {
    if (instance) {
        throw "Only one instance plz!";
    }

    createPlatform();
    createDevice();
    createContext();
    createCommandQueue();

    printOpenCLInfo();

    _program = createProgram("kernels/programs.cl");

    instance = this;
}

CLWrapper::~CLWrapper() {
    if (instance) {
        clReleaseCommandQueue(_cqueue);
        clReleaseContext(_context);
    }
    instance = 0;
}

char *CLWrapper::getPlatformInfo(cl_platform_info paramName) {
    size_t infoSize = 0;
    CL_SAFE_CALL(clGetPlatformInfo(_platform, paramName, 0, NULL, &infoSize));
    char *info = (char *) malloc(infoSize);
    CL_SAFE_CALL(clGetPlatformInfo(_platform, paramName, infoSize, info, NULL));
    return info;
}

void CLWrapper::createPlatform() {
    CL_SAFE_CALL(clGetPlatformIDs(1, &_platform, NULL));
}

void *CLWrapper::getDeviceInfo(cl_device_info paramName) {
    size_t infoSize = 0;
    CL_SAFE_CALL(clGetDeviceInfo(_device_id, paramName, 0, NULL, &infoSize));
    char *info = (char *) malloc(infoSize);
    CL_SAFE_CALL(clGetDeviceInfo(_device_id, paramName, infoSize, info, NULL));
    return info;
}

void CLWrapper::createDevice() {
    CL_SAFE_CALL(clGetDeviceIDs(_platform, _device_type, 1, &_device_id, NULL));
}

void CLWrapper::createContext() {
    _context = clCreateContext(0, 1, &_device_id, NULL, NULL, NULL);
    if (!_context) {
        std::cerr << "Context creation failed!" << std::endl;
        exit(EXIT_FAILURE);
    }
}

void CLWrapper::createCommandQueue() {
    _cqueue = clCreateCommandQueue(_context, _device_id, CL_QUEUE_PROFILING_ENABLE, NULL);
    if (!_cqueue) {
        std::cerr << "Command queue creation failed!" << std::endl;
    }
}

void CLWrapper::printOpenCLInfo() {
    std::cout << getPlatformInfo(CL_PLATFORM_VERSION) << std::endl;

    cl_uint *max_compute_units = (cl_uint *) getDeviceInfo(CL_DEVICE_MAX_COMPUTE_UNITS);
    std::cout << "Max compute units: " << *max_compute_units << std::endl;
}

bool CLWrapper::fileToString(const char *path, char *&out, int &len) {
    std::ifstream file(path, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    len = file.tellg();
    out = new char[len + 1];
    file.seekg(0, std::ios::beg);
    file.read(out, len);
    file.close();
    out[len] = 0;
    return true;
}

cl_program CLWrapper::createProgram(const char *fileName) {
    char *programSource = NULL;
    int len = 0;
    int errorFlag = -1;
    if (!fileToString(fileName, programSource, len)) {
        std::cerr << "Error loading program: " << fileName << std::endl;
        exit(EXIT_FAILURE);
    }
    cl_program program = 0;
    cl_int err;
    program = clCreateProgramWithSource(_context, 1, (const char **) &programSource, NULL, &err);
    if (!program) {
        std::cerr << "Error: Failed to create compute program!" << std::endl;
        switch (err) {
            case CL_INVALID_CONTEXT:
                std::cout << "context is not a valid context.\n";
                break;
            case CL_INVALID_VALUE:
                std::cout << "count is zero or if strings or any entry in strings is NULL.\n";
                break;
            case CL_OUT_OF_HOST_MEMORY:
                std::cout <<
                "there is a failure to allocate resources required by the OpenCL implementation on the host.\n";
                break;
        }
        exit(EXIT_FAILURE);
    }

    err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    if (err != CL_SUCCESS) {
        size_t len;
        char buffer[2048];

        std::cerr << "Error: Failed to build program executable!" << std::endl;
        clGetProgramBuildInfo(program, _device_id, CL_PROGRAM_BUILD_LOG,
                              sizeof(buffer), buffer, &len);
        std::cerr << buffer << std::endl;
        exit(1);
    }
    return program;
}

cl_kernel CLWrapper::createKernel(cl_program program, const char *kernelName) {
    cl_kernel kernel;
    cl_int err;
    kernel = clCreateKernel(program, kernelName, &err);

    switch (err) {
        case CL_INVALID_PROGRAM:
            std::cout << "program is not a valid program object.\n";
            break;
        case CL_INVALID_PROGRAM_EXECUTABLE:
            std::cout << "there is no successfully built executable for program.\n";
            break;
        case CL_INVALID_KERNEL_NAME:
            std::cout << "kernel_name is not found in program.\n";
            break;
        case CL_INVALID_KERNEL_DEFINITION:
            std::cout <<
            "the function definition for __kernel function given by kernel_name such as the number of arguments, the argument types are not the same for all devices for which the program executable has been built.\n";
            break;
        case CL_INVALID_VALUE:
            std::cout << "kernel_name is NULL.\n";
            break;
        case CL_OUT_OF_HOST_MEMORY:
            std::cout <<
            "there is a failure to allocate resources required by the OpenCL implementation on the host.\n";
            break;
    }


    if (!kernel || err != CL_SUCCESS) {
//  clGetProgramBuildInfo(program, _device_id, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &length);
        std::cerr << "Error: Failed to create compute kernel: " << kernelName << "!" << std::endl;
        exit(1);
    }
    return kernel;
}
