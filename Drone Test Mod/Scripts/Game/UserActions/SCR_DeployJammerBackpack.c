class SCR_DeployJammerBackpack :  ScriptedUserAction
{
	
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{

		
		
		
		
		
		
		JammerBackPack.Cast(pOwnerEntity.FindComponent(JammerBackPack)).SetJammerActive(true);
	}
	
	override bool CanBeShownScript(IEntity user){
	
	return !JammerBackPack.Cast(GetOwner().FindComponent(JammerBackPack)).GetJammerActive();
	}
}
