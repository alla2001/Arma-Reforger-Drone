

[ComponentEditorProps(category: "Tutorial/Component", description: "TODO")]
class DroneWingSpineClass : ScriptComponentClass
{
}

class DroneWingSpine : ScriptComponent // GameComponent > GenericComponent
{
	[Attribute()]
	float speedMult;
	 [RplProp(condition: RplCondition.NoOwner)]
	float spinSpeed;
	
	  override void OnPostInit(IEntity owner)
    {
        super.OnPostInit(GetOwner());
     
        SetEventMask(owner, EntityEvent.POSTFRAME);

		
        return;
    }
	
	override void EOnPostFrame(IEntity owner, float timeSlice)
	{
		vector tran[4];
		owner.GetTransform(tran);
		vector tran1[4];
		SCR_Math3D.RotateAround(tran,owner.GetOrigin(), tran[1], timeSlice*spinSpeed+timeSlice*1, tran1);
		owner.SetTransform(tran1);
	}
	
	void SetSpinSpeed(float speed){
		spinSpeed = speed;
		Replication.BumpMe();
	}
	
}