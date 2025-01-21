

[ComponentEditorProps(category: "Tutorial/Component", description: "TODO")]
class CustomSCR_PickUpItemActionClass : SCR_PickUpItemAction
{
}

class CustomSCR_PickUpItemAction : SCR_PickUpItemAction // GameComponent > GenericComponent
{
	
	override protected void PerformActionInternal(SCR_InventoryStorageManagerComponent manager, IEntity pOwnerEntity, IEntity pUserEntity)
	{
		super.PerformActionInternal(manager, pOwnerEntity, pUserEntity);
		
		DroneController.Cast(GetOwner().FindComponent(DroneController)).PickUp();
	}
	
	override bool CanBeShownScript(IEntity user){
	
		
		return (!DroneController.Cast(GetOwner().FindComponent(DroneController)).isDeployed()&&super.CanBeShownScript(user));
	}
}