

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
	vector localPos;

	override void OnPostInit(IEntity owner)
    {
        super.OnPostInit(GetOwner());
     
        SetEventMask(owner, EntityEvent.POSTFRAME);

		localPos = owner.CoordToLocal( owner.GetOrigin());

        return;
    }
	
	override void EOnPostFrame(IEntity owner, float timeSlice)
	{
	vector tran[4];
owner.GetTransform(tran);

// Extract the current scale
float scale = tran[0].Length(); // Assuming uniform scaling

// Perform the rotation
vector tran1[4];
SCR_Math3D.RotateAround(tran, owner.GetOrigin(), tran[1], timeSlice * spinSpeed + timeSlice * 5, tran1);

// Reapply the scale
tran1[0] = tran1[0].Normalized() * scale;
tran1[1] = tran1[1].Normalized() * scale;
tran1[2] = tran1[2].Normalized() * scale;

// Set the transformed matrix back to the object
owner.SetTransform(tran1);
	owner.SetOrigin( owner.CoordToParent( localPos));
		owner.Update();
			//Print(spinSpeed);
	}
	
	void SetSpinSpeed(float speed){
	
			spinSpeed = speed;
	Rpc( RPC_SetSpinSpeed,spinSpeed);
	}
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RPC_SetSpinSpeed(float speed){
	
		spinSpeed = speed;
		Replication.BumpMe();
	}
	void ToggleMesh(bool val){
	
		GetOwner().Show(val);
	}
	
}