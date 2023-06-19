#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <vector>



DWORD GetProcId(const wchar_t* ProcName) {
	HANDLE hproc = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hproc != INVALID_HANDLE_VALUE) {
		PROCESSENTRY32 pe;
		pe.dwSize = sizeof(pe);
		if (Process32First(hproc, &pe)) {
			do {
				if (!_wcsicmp(ProcName, pe.szExeFile)) {
					
					return pe.th32ProcessID;
				}
			}while (Process32Next(hproc, &pe));
		}
		
	}
}

uintptr_t GetModuleBaseAddr(DWORD pid, const wchar_t* ModName) {
	HANDLE hmod = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
	if (hmod != INVALID_HANDLE_VALUE) {
		MODULEENTRY32 me;
		me.dwSize = sizeof(me);
		if (Module32First(hmod, &me)) {
			do {
				if (!_wcsicmp(me.szModule, ModName)) {
				
					return (uintptr_t)me.modBaseAddr;
				}
			} while (Module32Next(hmod, &me));
		}
	}
}

uintptr_t FindDMAAddry(HANDLE handp, uintptr_t ptr, std::vector<unsigned int> off){
	uintptr_t addr = ptr;
	for (int i = 0; i < off.size(); i++) {
		ReadProcessMemory(handp, (BYTE*)addr, &addr, 4, nullptr);
		addr += off[i];
	}
	return addr;
}

int main() {
	DWORD pid = GetProcId(L"ac_client.exe");

	uintptr_t modbaseaddr = GetModuleBaseAddr(pid, L"ac_client.exe");

	HANDLE hproc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

	uintptr_t lplayerbaseaddr = modbaseaddr + 0x195404;

	std::vector<unsigned int> offsets = { 0x0374 , 0x014 , 0x0 };
	uintptr_t ammooffset = FindDMAAddry(hproc, lplayerbaseaddr, offsets);
	

	
	DWORD ammo;
	ReadProcessMemory(hproc, (LPCVOID*)ammooffset , &ammo, sizeof(ammo), nullptr);
	std::cout << "ammo adress " << std::hex << ammooffset << std::endl;
	std::cout << "Current ammo " << std::dec << ammo << std::endl;
	

	DWORD newammo = 30;
	DWORD infammo = 9999;
	std::cout << "Press F5 to reset ammo to 30" << std::endl;
	std::cout << "Press F6 to get infinite ammo" << std::endl;
	while (true) {
		if (GetAsyncKeyState(VK_F5)!=0){
			WriteProcessMemory(hproc, (LPVOID*)ammooffset, &newammo, sizeof(newammo), nullptr);
		}
		if (GetAsyncKeyState(VK_F6) != 0) {
			WriteProcessMemory(hproc, (LPVOID*)ammooffset, &infammo, sizeof(infammo), nullptr);
		}
	}

	return 0;
}