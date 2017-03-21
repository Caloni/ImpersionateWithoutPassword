// ImpersionateWithoutPassword.cpp : Creates a process using a user account without knowing the password.
//

#include "stdafx.h"
#include <Windows.h>
#include <Wtsapi32.h>
#include <Userenv.h>
#include <stdio.h>

#pragma comment(lib, "Wtsapi32.lib")
#pragma comment(lib, "Userenv.lib")


int main(int argc, char* argv[])
{
	while (!IsDebuggerPresent())
		Sleep(1000);

	if (argc == 2)
	{
		ULONG id = atoi(argv[1]);
		HANDLE token = NULL;

		if (HANDLE proc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, id))
		{
			OpenProcessToken(proc, TOKEN_ALL_ACCESS, &token);
		}

		if (token == NULL)
			WTSQueryUserToken(id, &token);

		if( token != NULL)
		{
			HANDLE tokenDup;

			if (DuplicateTokenEx(token, MAXIMUM_ALLOWED, NULL, SecurityIdentification, TokenPrimary, &tokenDup))
			{
				PVOID env = nullptr;
				if (CreateEnvironmentBlock(&env, tokenDup, TRUE))
				{
					STARTUPINFOW si = { sizeof(si) };
					PROCESS_INFORMATION pi = {};
					WCHAR cmd[MAX_PATH] = L"c:\\WINDOWS\\notepad.exe";

					if (CreateProcessAsUserW(tokenDup, cmd, NULL, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi))
					{
						printf("Sucess creating new process!\n");
					}
					else printf("Error %d in CreateProcessAsUserW\n", GetLastError());
				}
				else printf("Error %d in CreateEnvironmentBlock\n", GetLastError());
			}
			else printf("Error %d in DuplicateTokenEx\n", GetLastError());
		}
		else printf("Error %d in WTSQueryUserToken\n", GetLastError());
	}
	else printf("How to use: <session-id/process-id>\n");

    return 0;
}

