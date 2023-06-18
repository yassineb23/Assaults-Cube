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

uintptr_t FindDMAAddry(HANDLE hproc, uintptr_t ptr, std::vector<unsigned int> offsets){
	uintptr_t addr = ptr;
	for (int i = 0; i < offsets.size(); i++) {
		ReadProcessMemory(hproc, (BYTE*)addr, &addr, sizeof(addr), nullptr);
		addr += offsets[i];
	}

	return addr;
}

int main() {
	DWORD pid = GetProcId(L"ac_client.exe");

	uintptr_t modbaseaddr = GetModuleBaseAddr(pid, L"ac_client.exe");

	HANDLE hproc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

	uintptr_t lplayerbaseaddr = modbaseaddr + 0x10F4F4;
	
	uintptr_t ammooffset = lplayerbaseaddr + 0x0150;

	/*OFFSETS WRONG*//*OFFSETS WRONG*//*OFFSETS WRONG*/
	
	DWORD ammo;
	ReadProcessMemory(hproc,(BYTE*)ammooffset, &ammo, sizeof(ammo), nullptr);
	std::cout << "Current ammo " << ammo;

	DWORD newammo = 30;
	while (true) {
		if (GetAsyncKeyState(VK_F5)!=0){
			WriteProcessMemory(hproc, (BYTE*)ammooffset, &newammo, sizeof(newammo), nullptr);
		}
	}

	return 0;
}