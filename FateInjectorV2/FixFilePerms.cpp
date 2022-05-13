#include "pch.h"
#include "FixFilePerms.h"



//	WARNING: This code is shamelessly stolen from Wunkolo's UWPDdumper project :|
//	https://github.com/Wunkolo/UWPDumper
//	go to code :P 
//	https://github.com/Wunkolo/UWPDumper/blob/9fb0a040e674521c1413276bcea6e4e708f34d19/UWPInjector/source/main.cpp#L226 17.10.2020




// Setting DLL access controls
#include <stdio.h>
#include <string>
#include <Aclapi.h>
#include <Sddl.h>

void SetAccessControl(std::wstring &ExecutableName, const wchar_t* AccessString)
{
	PSECURITY_DESCRIPTOR SecurityDescriptor = nullptr;
	EXPLICIT_ACCESSW ExplicitAccess = { 0 };

	ACL* AccessControlCurrent = nullptr;
	ACL* AccessControlNew = nullptr;

	SECURITY_INFORMATION SecurityInfo = DACL_SECURITY_INFORMATION;
	PSID SecurityIdentifier = nullptr;

	if (
		GetNamedSecurityInfoW(
			ExecutableName.c_str(),
			SE_FILE_OBJECT,
			DACL_SECURITY_INFORMATION,
			nullptr,
			nullptr,
			&AccessControlCurrent,
			nullptr,
			&SecurityDescriptor
		) == ERROR_SUCCESS
		)
	{
		ConvertStringSidToSidW(AccessString, &SecurityIdentifier);
		if (SecurityIdentifier != nullptr)
		{
			ExplicitAccess.grfAccessPermissions = GENERIC_READ | GENERIC_EXECUTE;
			ExplicitAccess.grfAccessMode = SET_ACCESS;
			ExplicitAccess.grfInheritance = SUB_CONTAINERS_AND_OBJECTS_INHERIT;
			ExplicitAccess.Trustee.TrusteeForm = TRUSTEE_IS_SID;
			ExplicitAccess.Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
			ExplicitAccess.Trustee.ptstrName = reinterpret_cast<wchar_t*>(SecurityIdentifier);

			if (
				SetEntriesInAclW(
					1,
					&ExplicitAccess,
					AccessControlCurrent,
					&AccessControlNew
				) == ERROR_SUCCESS
				)
			{
				SetNamedSecurityInfoW(
					const_cast<wchar_t*>(ExecutableName.c_str()),
					SE_FILE_OBJECT,
					SecurityInfo,
					nullptr,
					nullptr,
					AccessControlNew,
					nullptr
				);
			}
		}
	}
	if (SecurityDescriptor)
	{
		LocalFree(reinterpret_cast<HLOCAL>(SecurityDescriptor));
	}
	if (AccessControlNew)
	{
		LocalFree(reinterpret_cast<HLOCAL>(AccessControlNew));
	}
}