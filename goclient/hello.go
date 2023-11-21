package main

import (
    "fmt"
    "github.com/mattn/gowasmer/wasmutil"
    "syscall/js"
)

// export hello
func hello(value js.Value, args []js.Value) interface{} {
    fmt.Println("Golang: hello(): Hello, World!")
}

func main() {
    fmt.Println("Golang: Main: Hello, World!")
    c := make(chan struct{})
    js.Global().Set("hello", js.FuncOf(wasmutil.Wrap(hello)))
    <-c
}
