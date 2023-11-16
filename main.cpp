// Install includes and libs by running:
// scoop install wasmer
// 
// The install sets the environment variable: WASMER_DIR
//
// Then restart Visual Studio to compile
//
// https://wasmtime.dev/ - Run installer

#include <fstream>
#include <stdio.h>
#include "wasmer.h"


// https://github.com/bytecodealliance/wasmtime/releases
// https://github.com/bytecodealliance/wasmtime-cpp

#pragma comment( lib, "Wldap32" )
#pragma comment( lib, "Ws2_32" )
#pragma comment (lib, "crypt32")
//#pragma comment (lib, "jsoncpp")
#pragma comment(lib, "wsock32")
#pragma comment(lib, "iphlpapi")
#pragma comment(lib, "userenv")
#pragma comment(lib, "psapi")
#pragma comment(lib, "ntdll")
#pragma comment(lib, "bcrypt")

#pragma comment(lib, "wasmtime.dll.lib")
#pragma comment(lib, "wasmer.lib")

int main(int argc, const char* argv[]) {
    const char* wasm_file_path = "wasm/client.wasm"; // Replace with the actual path to your wasm file

    // Read the contents of the WebAssembly file
    FILE* wasm_file = NULL;
    if (fopen_s(&wasm_file, wasm_file_path, "rb") != 0 || wasm_file == NULL) {
        printf("> Error opening WebAssembly file!\n");
        return 1;
    }

    fseek(wasm_file, 0, SEEK_END);
    long file_size = ftell(wasm_file);
    fseek(wasm_file, 0, SEEK_SET);

    wasm_byte_vec_t wasm_bytes = { 0 };
    wasm_byte_vec_new_uninitialized(&wasm_bytes, file_size);
    fread(wasm_bytes.data, 1, file_size, wasm_file);
    fclose(wasm_file);


    printf("Creating the store...\n");
    wasm_engine_t* engine = wasm_engine_new();
    wasm_store_t* store = wasm_store_new(engine);

    printf("Compiling module...\n");
    wasm_module_t* module = wasm_module_new(store, &wasm_bytes);

    if (!module) {
        printf("> Error compiling module!\n");
        wasm_byte_vec_delete(&wasm_bytes);
        return 1;
    }

    wasm_byte_vec_delete(&wasm_bytes);

    printf("Creating imports...\n");
    wasm_extern_vec_t import_object = WASM_EMPTY_VEC;

    printf("Instantiating module...\n");
    wasm_instance_t* instance = wasm_instance_new(store, module, &import_object, NULL);

    if (!instance) {
        printf("> Error instantiating module!\n");
        wasm_byte_vec_delete(&wasm_bytes);
        return 1;
    }

    printf("Retrieving exports...\n");
    wasm_extern_vec_t exports;
    wasm_instance_exports(instance, &exports);

    if (exports.size == 0) {
        printf("> Error accessing exports!\n");
        wasm_byte_vec_delete(&wasm_bytes);
        return 1;
    }

    printf("Retrieving the `sum` function...\n");
    wasm_func_t* sum_func = wasm_extern_as_func(exports.data[0]);

    if (sum_func == NULL) {
        printf("> Failed to get the `sum` function!\n");
        wasm_byte_vec_delete(&wasm_bytes);
        return 1;
    }

    printf("Calling `sum` function...\n");
    wasm_val_t args_val[2] = { WASM_I32_VAL(3), WASM_I32_VAL(4) };
    wasm_val_t results_val[1] = { WASM_INIT_VAL };
    wasm_val_vec_t args = WASM_ARRAY_VEC(args_val);
    wasm_val_vec_t results = WASM_ARRAY_VEC(results_val);

    if (wasm_func_call(sum_func, &args, &results)) {
        printf("> Error calling the `sum` function!\n");
        wasm_byte_vec_delete(&wasm_bytes);
        return 1;
    }

    printf("Results of `sum`: %d\n", results_val[0].of.i32);

    wasm_module_delete(module);
    wasm_extern_vec_delete(&exports);
    wasm_instance_delete(instance);
    wasm_store_delete(store);
    wasm_engine_delete(engine);
    wasm_byte_vec_delete(&wasm_bytes);

    return 0;
}
