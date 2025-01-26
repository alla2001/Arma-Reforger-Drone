class SCR_UnDeployJammerBackpack :  ScriptedUserAction
{
	
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{

		
		
		
		
		
		
		JammerBackPack.Cast(pOwnerEntity.FindComponent(JammerBackPack)).SetJammerActive(false);
	}
	
	override bool CanBeShownScript(IEntity user){
	
	return JammerBackPack.Cast(GetOwner().FindComponent(JammerBackPack)).GetJammerActive();
	}
}
