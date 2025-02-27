class SCR_DeployDroneAction :  ScriptedUserAction
{
	
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{

		
			if(LIMod.canfly=="no"||LIMod.canfly2=="ni" )
		return;
		
		
		if(pOwnerEntity.FindComponent(DroneController)!=null)
		
		DroneController.Cast(pOwnerEntity.FindComponent(DroneController)).DeployDrone(pOwnerEntity,pUserEntity);
		

	}
	
	override bool CanBePerformedScript(IEntity user){
	
		if(LIMod.canfly=="no"||LIMod.canfly2=="ni" )
		return false;
		DroneBatteryHandler droneBattery = DroneBatteryHandler.Cast(GetOwner().FindComponent(DroneBatteryHandler));
	if(droneBattery==null||droneBattery.isDead())
		{
		SetCannotPerformReason("Battery Out");
			return false;
		}
		
		return true;
	}
	
	override bool CanBeShownScript(IEntity user){
			if(LIMod.canfly=="no"||LIMod.canfly2=="ni" )
		return false;
	if(DroneController.Cast(GetOwner().FindComponent(DroneController)).isDeployed())
		{
		SetCannotPerformReason("Already Deployed");
			return false;
		}
	

		return true;
	}
}
