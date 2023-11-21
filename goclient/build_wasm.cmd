REM
REM Run the following script in the Windows Command Prompt (Not Powershell) to build the WebAssembly module
REM
REM Dpendencies:
REM
REM Install Scoop, the CLI Windows Installer: (Use PowerShell)
REM Set-ExecutionPolicy RemoteSigned -Scope CurrentUser # Optional: Needed to run a remote script the first time
REM irm get.scoop.sh | iex
REM
REN Run in the Windows Command Prompt
REM Install TinyGO - The Go Compiler
REM scoop install tinygo
REM
REM Add WebAssembly/WASI compiler support
REM scoop install binaryen
REM

go mod init goclient
go get github.com/mattn/gowasmer/wasmutil

SET GOOS=js
SET GOARCH=wasm
CALL go build -o ..\wasm\hello.wasm hello.go

wasmer run ..\wasm\hello.wasm
