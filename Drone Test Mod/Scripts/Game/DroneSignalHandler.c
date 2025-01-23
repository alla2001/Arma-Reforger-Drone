

[ComponentEditorProps(category: "Tutorial/Component", description: "TODO")]
class DroneSignalHandlerClass : ScriptComponentClass
{
}

class DroneSignalHandler : ScriptComponent // GameComponent > GenericComponent
{
	

	float currentSignalStrength = 1.0;
	[Attribute("100")]
	float maxRange;

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
 
	
        return;
    }
	 override void EOnInit(IEntity owner)
    {
		
	
		
		
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
	}
	void SetControlingPlayer(IEntity playerOwner){
	controlingPlayer =   playerOwner;
	}
	void Disconnected(){
		controlingPlayer=null;
		delete root;
	}
	
	void CalcualteSignalStrength()
	{
		float dist =vector.Distance( GetOwner().GetOrigin(),controlingPlayer.GetOrigin());
		currentSignalStrength=Math.Clamp(((maxRange-dist)/maxRange),0,1);
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
	  override void EOnSimulate(IEntity owner, float timeSlice)
    {
		  bool m_bIsServer = Replication.IsServer();
		 if (m_bIsServer&&controlingPlayer)
		CalcualteSignalStrength();
	}
override void OnDelete(IEntity owner){
		if(root)
		delete root;
	
	}
	
}