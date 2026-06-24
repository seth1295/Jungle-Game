using UnrealBuildTool;
using System.Collections.Generic;

public class JungleGameTarget : TargetRules
{
	public JungleGameTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V7;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("JungleGame");
	}
}
