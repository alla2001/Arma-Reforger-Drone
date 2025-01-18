

[ComponentEditorProps(category: "Tutorial/Component", description: "TODO")]
class DroneSignalHandlerClass : ScriptComponentClass
{
}

class DroneSignalHandler : ScriptComponent // GameComponent > GenericComponent
{
	

	float currentSignalStrength;
	[Attribute("100")]
	float maxRange;

	SCR_CameraPostProcessEffect filmGrainPostEffect;
	IEntity controlingPlayer;
	SCR_PostProcessCameraComponent post;

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
	void Deploy(IEntity playerOwner,IEntity Camera){
	 post=SCR_PostProcessCameraComponent.Cast( Camera
		
		.FindComponent(SCR_PostProcessCameraComponent));
		filmGrainPostEffect = post.FindEffect(PostProcessEffectType.FilmGrain);
	
		if (!filmGrainPostEffect)
		{
			Print(string.Format("Error when loading post-process material '%1'! Please check effect priority.", filmGrainPostEffect), LogLevel.ERROR);

			return;
		}
	
	controlingPlayer =   playerOwner;
		
	}
	
	void CalcualteSignalStrength()
	{
		float dist =vector.Distance( GetOwner().GetOrigin(),controlingPlayer.GetOrigin());
		currentSignalStrength=Math.AbsFloat(1-((maxRange-dist)/maxRange));
		if (filmGrainPostEffect)
			filmGrainPostEffect.SetParam("Intensity",currentSignalStrength );
		Print(currentSignalStrength);
	}

	  override void EOnSimulate(IEntity owner, float timeSlice)
    {
		if(controlingPlayer)
		CalcualteSignalStrength();
	}

	
}