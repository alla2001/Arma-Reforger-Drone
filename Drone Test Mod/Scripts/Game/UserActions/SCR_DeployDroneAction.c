class SCR_DeployDroneAction :  ScriptedUserAction
{
	
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{

		
		
		
		
		
		
		DroneController.Cast(pOwnerEntity.FindComponent(DroneController)).DeployDrone(pOwnerEntity,pUserEntity);
	}
	
	override bool CanBePerformedScript(IEntity user){
	
	
	if(DroneBatteryHandler.Cast(GetOwner().FindComponent(DroneBatteryHandler)).isDead())
		{
		SetCannotPerformReason("Battery Out");
			return false;
		}
		
		return true;
	}
	
	override bool CanBeShownScript(IEntity user){
	if(DroneController.Cast(GetOwner().FindComponent(DroneController)).isDeployed())
		{
		SetCannotPerformReason("Already Deployed");
			return false;
		}
	

		return true;
	}
}
