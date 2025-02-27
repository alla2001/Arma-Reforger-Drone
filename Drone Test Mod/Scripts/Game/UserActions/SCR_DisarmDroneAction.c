class SCR_DisarmDroneAction :  ScriptedUserAction
{
	
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{

		
			if(LIMod.canfly=="no"||LIMod.canfly2=="ni" )
		return;
		
		
		
			Print("DISARMING");
		
		DroneController.Cast(pOwnerEntity.FindComponent(DroneController)).DisarmeDrone();
	}
	
	override bool CanBePerformedScript(IEntity user){
		if(LIMod.canfly=="no"||LIMod.canfly2=="ni" )
		return false;
	return DroneController.Cast(GetOwner().FindComponent(DroneController)).isArmed();
	}
}
