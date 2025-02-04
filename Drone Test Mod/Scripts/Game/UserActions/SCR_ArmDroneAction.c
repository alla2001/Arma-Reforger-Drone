class SCR_ArmDroneAction :  ScriptedUserAction
{
	
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{

		
		
		
		
		
			Print("ARMING");
		DroneController.Cast(pOwnerEntity.FindComponent(DroneController)).ArmDrone();
	}
	
	override bool CanBePerformedScript(IEntity user){
	
	return !DroneController.Cast(GetOwner().FindComponent(DroneController)).isArmed();
	}
}
