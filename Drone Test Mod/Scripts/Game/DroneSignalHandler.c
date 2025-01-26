

[ComponentEditorProps(category: "Tutorial/Component", description: "TODO")]
class DroneSignalHandlerClass : ScriptComponentClass
{
}

class DroneSignalHandler : ScriptComponent // GameComponent > GenericComponent
{
	

	float currentSignalStrength = 1.0;
	[Attribute("100")]
	float maxRange;
	float debuff=0;
	SCR_CameraPostProcessEffect filmGrainPostEffect;
	IEntity controlingPlayer;
	SCR_PostProcessCameraComponent post;
	ImageWidget Bar1;
	ImageWidget Bar2;
	ImageWidget Bar3;
	[Attribute()] ResourceName layout;
	[Attribute()]
	ref Color WarningColor;
	ref Color defaultColor;
   	Widget root ;
	
	 override void OnPostInit(IEntity owner)
    {
        super.OnPostInit(GetOwner());
      
        SetEventMask(owner, EntityEvent.SIMULATE);
        SetEventMask(owner, EntityEvent.INIT);
 
	RegisterDrone();
    }
	override void EOnInit(IEntity owner)
    {
		
	RegisterDrone();
		
    }
	void Deploy(IEntity playerOwner,IEntity Camera)
	{
	 	post=SCR_PostProcessCameraComponent.Cast( Camera
		
		.FindComponent(SCR_PostProcessCameraComponent));
		filmGrainPostEffect = post.FindEffect(PostProcessEffectType.FilmGrain);
	
		if (!filmGrainPostEffect)
		{
			Print(string.Format("Error when loading post-process material '%1'! Please check effect priority.", filmGrainPostEffect), LogLevel.ERROR);

			return;
		}
	
		
		root = GetGame().GetWorkspace().CreateWidgets(layout);

  
  
		 Bar1= ImageWidget.Cast(	root.FindAnyWidget("Image0"));
		 Bar2= ImageWidget.Cast(	root.FindAnyWidget("Image1"));
		 Bar3= ImageWidget.Cast(	root.FindAnyWidget("Image2"));
		defaultColor= Bar1.GetColor();
			RegisterDrone();
	}
	void SetControlingPlayer(IEntity playerOwner){
	controlingPlayer =   playerOwner;
	}
	void Disconnected(){
		controlingPlayer=null;
			UnRegisterDrone();
		delete root;
	}
	void SetDebuff(float debuffVal){
	
	debuff=Math.Clamp(debuffVal,0,1);
	}
	void CalcualteSignalStrength()
	{
		float dist =vector.Distance( GetOwner().GetOrigin(),controlingPlayer.GetOrigin());
		currentSignalStrength=Math.Clamp(((maxRange-dist)/maxRange)*(1-debuff),0,1);
		Rpc(RPC_UpdateSignal,currentSignalStrength);
	}
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	void RPC_UpdateSignal(float signal){
	
		currentSignalStrength=signal;
			if (filmGrainPostEffect)
			filmGrainPostEffect.SetParam("Intensity",(0.7)-(currentSignalStrength)+0.1 );
		if(!root)return;
		if(currentSignalStrength > 0.1)
		Bar1.SetColor(defaultColor);
		else
		Bar1.SetColor(WarningColor);
		Bar2.SetVisible(currentSignalStrength>0.33);
		Bar3.SetVisible(currentSignalStrength>0.66);
	}
	bool isOutOfRange(){return currentSignalStrength<=0;}
		bool isjammed(){return debuff>=1;}
	  override void EOnSimulate(IEntity owner, float timeSlice)
    {
		  bool m_bIsServer = Replication.IsServer();
		 if (m_bIsServer)
		CalcualteSignalStrength();
	}
override void OnDelete(IEntity owner){
		if(root)
		delete root;
	
	UnRegisterDrone();
	
	}
	void RegisterDrone(){
		if(this==null)return;
		Game game = GetGame();
			if(game==null)return;
		World world =  game.GetWorld();
		if(world==null)return;
	    JammerSystem jammerSys= JammerSystem.Cast( world.FindSystem(JammerSystem));
		if(jammerSys==null)return;
		jammerSys.RegisterDrone(this);
	}
	void UnRegisterDrone(){
		if(this==null)return;
		Game game = GetGame();
			if(game==null)return;
		World world =  game.GetWorld();
		if(world==null)return;
	    JammerSystem jammerSys= JammerSystem.Cast( world.FindSystem(JammerSystem));
		if(jammerSys==null)return;
			jammerSys.UnregisterDrone(this);
	}
}