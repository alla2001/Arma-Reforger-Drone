

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
	SignalsManagerComponent procAnima;
	override void OnPostInit(IEntity owner)
    {
        super.OnPostInit(GetOwner());
     
        SetEventMask(owner, EntityEvent.POSTFRAME);

		localPos = owner.CoordToLocal( owner.GetOrigin());
procAnima=SignalsManagerComponent.Cast( owner.FindComponent(SignalsManagerComponent));
        return;
    }
	
	override void EOnPostFrame(IEntity owner, float timeSlice)
	{
		if(procAnima){
		procAnima.SetSignalValue( procAnima.FindSignal("SpinRPM"),10);
			procAnima.SetSignalValue( procAnima.FindSignal("DroneOn"),1);
		return;
		}
				owner.SetOrigin( owner.CoordToParent( localPos));
		
			owner.Update();
	vector tran[4];
	owner.GetTransform(tran);

	// Extract the current scale
	float scale = tran[0].Length(); // Assuming uniform scaling

	// Perform the rotation
	vector tran1[4];
	SCR_Math3D.RotateAround(tran, tran[3], tran[1].Normalized(), timeSlice * spinSpeed + timeSlice * 5, tran);
/*
	// Reapply the scale
	tran1[0] = tran1[0].Normalized() * scale;
	tran1[1] = tran1[1].Normalized() * scale;
	tran1[2] = tran1[2].Normalized() * scale;
*/
		// Set the transformed matrix back to the object
		owner.SetTransform(tran);

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