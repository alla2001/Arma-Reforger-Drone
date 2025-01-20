class SoundCompClass: SoundComponentClass
{
}

class SoundComp: SoundComponent
{

	override void OnPostInit(IEntity owner)
	{	
		super.OnPostInit(owner);
		
		SoundEvent("SOUND_FLY");
			
		
	}
	override void OnFrame(IEntity owner, float timeSlice){
	
	SoundEvent("SOUND_FLY");
	
	}
		
}