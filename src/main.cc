#include "blook/blook.h"
#include "blook/misc.h"
#include <print>

#include <Windows.h>
#include <algorithm>

int main() {
  try {
    // change cwd to the directory of the executable
    auto path = std::filesystem::path(__argv[0]);
    std::filesystem::current_path(path.parent_path());

    // start process with the given name
    auto names = std::vector<std::string>{"AyuGram.exe", "Telegram.exe",
                                          "Telegram Desktop.exe", "64gram.exe"};
    auto name = std::ranges::find_if(
        names, [](const auto &name) { return std::filesystem::exists(name); });

    if (name == names.end()) {
      return -1;
    }

    PROCESS_INFORMATION pi{};
    STARTUPINFOA si{};
    if (!CreateProcessA(name->c_str(), nullptr, 0, 0, FALSE, 0, nullptr,
                        nullptr, &si, &pi)) {
      return -1;
    }

    auto proc = blook::Process::attach(pi.dwProcessId);
    std::print("pid: {}\n", pi.dwProcessId);
    while (!proc->module("kernelbase.dll").has_value()) {
      Sleep(10);
    }
    std::print("kernelbase.dll loaded\n");
    proc->module("kernelbase.dll")
        .value()
        ->exports("GetUserDefaultLCID")
        .value()
        .pointer()
        .reassembly([](auto a) {
          a.mov(zasm::x86::rax, zasm::Imm(0x0804));
          a.ret();
        })
        .patch();
  } catch (const std::exception &e) {
    std::print("Error: {}\n", e.what());
  }

  std::println("Injected");
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {
  main();
  return 0;
}