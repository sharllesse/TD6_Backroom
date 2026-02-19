/* Copyright (c) 2019 Unity Technologies.
 *
 * This software is subject to, and made available under, the Unity Terms of Service (see Unity Terms of Service).
 * Your use of this software constitutes your acceptance of such terms.

 * Unless expressly provided otherwise, the software under this license is made available strictly on an "AS IS" BASIS WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED.
 * Please review the Terms of Service for details on these and other terms and conditions.”
 */

using UnrealBuildTool;

public class VivoxCore : ModuleRules
{
	public VivoxCore(ReadOnlyTargetRules Target) : base(Target)
	{

		PublicIncludePaths.AddRange(
			new string[] {
				string.Format("{0}/Public", ModuleDirectory),
			}
			);


		PrivateIncludePaths.AddRange(
			new string[] {
                string.Format("{0}/Private", ModuleDirectory),
			}
			);


		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "CoreUObject",
                "VivoxCoreLibrary",
				"Projects",
                "Engine",
			}
			);


		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				// ... add private dependencies that you statically link with here ...
			}
			);


		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
            }
			);

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            string vivoxSdkDll = string.Format("{0}\\..\\ThirdParty\\VivoxCoreLibrary\\Windows\\Release\\x64\\vivoxsdk.dll", ModuleDirectory);
            if (!System.IO.File.Exists(vivoxSdkDll))
            {
                System.Diagnostics.Debug.WriteLine(string.Format("Warning: {0} does not exist", vivoxSdkDll));
            }
            RuntimeDependencies.Add(vivoxSdkDll);
        }

        // Build
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
    }
}
