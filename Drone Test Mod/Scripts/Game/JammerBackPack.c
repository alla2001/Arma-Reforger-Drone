



[ComponentEditorProps(category: "Tutorial/Component", description: "TODO")]
class JammerBackPackClass : ScriptComponentClass
{
}

class JammerBackPack : ScriptComponent // GameComponent > GenericComponent
{
	[Attribute("200")]
	float range;
	
	[Attribute("200")]
	float disconnectRange;
	[RplProp(condition: RplCondition.NoOwner)]
	bool enabled;
		
	 override void OnPostInit(IEntity owner)
    {
        super.OnPostInit(GetOwner());
   		SetEventMask(owner, EntityEvent.INIT);
			
		RegisterJammer();
        return;
    }
	 override void EOnInit(IEntity owner)
    {
		
		
    }
	override void OnDelete(IEntity owner){
	
		UnRegisterJammer();
	
	}
	void SetJammerActive(bool active){
		enabled=active;
		
		Rpc(RPC_SetJammerActive,active);
		if(active)
		RegisterJammer();
		else
		UnRegisterJammer();
	}
	bool GetJammerActive(){
		return enabled;
		
	}
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RPC_SetJammerActive(bool active)
	{
	
		enabled=active;
		Replication.BumpMe();
		if(active)
		RegisterJammer();
		else
		UnRegisterJammer();
	}
	void UnRegisterJammer(){
			if(this==null)return;
		World world =  GetGame().GetWorld();
			if(world==null)return;
	   	JammerSystem jammerSys= JammerSystem.Cast( world.FindSystem(JammerSystem));
				if(jammerSys==null)return;
		jammerSys.UnregisterJammer(this);
	}
	void RegisterJammer(){
	if(this==null)return;
		Game game = GetGame();
			if(game==null)return;
		World world =  game.GetWorld();
		if(world==null)return;
	   JammerSystem jammerSys= JammerSystem.Cast( world.FindSystem(JammerSystem));
				if(jammerSys==null)return;
		jammerSys.RegisterJammer(this);
	}
	
}