class SCR_DeployDroneAction :  ScriptedUserAction
{
	
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{

		
		
		
		
		
			Print("DEPLOYING");
		DroneController.Cast(pOwnerEntity.FindComponent(DroneController)).DeployDrone(pOwnerEntity,pUserEntity);
	}
	
	override bool CanBeShownScript(IEntity user){
	
	return !DroneController.Cast(GetOwner().FindComponent(DroneController)).isDeployed();
	}
}
