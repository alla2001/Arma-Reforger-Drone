

[ComponentEditorProps(category: "Tutorial/Component", description: "TODO")]
class CameraFollowDroneClass : ScriptComponentClass
{
}

class CameraFollowDrone : ScriptComponent // GameComponent > GenericComponent
{
	
	
	
	
	override void EOnPostFrame(IEntity owner, float timeSlice)
	{
		IEntity parent =  owner.GetParent();
		if(parent == null) return;
		
		Print("Parent : " + parent.GetName());
		//owner.SetOrigin(owner.GetParent().GetOrigin());
	}

	//------------------------------------------------------------------------------------------------
	protected override void OnPostInit(IEntity owner)
	{
			
		SetEventMask(owner, EntityEvent.POSTFRAME);
		
	}
}