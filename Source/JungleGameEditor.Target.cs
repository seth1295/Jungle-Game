using UnrealBuildTool;

public class JungleGameEditorTarget : TargetRules
{
	public JungleGameEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V7;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("JungleGame");
	}
}
