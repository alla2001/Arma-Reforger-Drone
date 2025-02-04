class SCR_DisarmDroneAction :  ScriptedUserAction
{
	
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{

		
		
		
		
		
			Print("DISARMING");
		
		DroneController.Cast(pOwnerEntity.FindComponent(DroneController)).DisarmeDrone();
	}
	
	override bool CanBePerformedScript(IEntity user){
	
	return DroneController.Cast(GetOwner().FindComponent(DroneController)).isArmed();
	}
}
