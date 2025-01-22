

[ComponentEditorProps(category: "Tutorial/Component", description: "TODO")]
class DroneBatteryHandlerClass : ScriptComponentClass
{
}

class DroneBatteryHandler : ScriptComponent // GameComponent > GenericComponent
{
	
	[Attribute("100")]
	float currentBattery;
	[Attribute("100")]
	float maxBattery;
	[Attribute("0.05")]
	float batteryConsumptionPerForceUnite;
	    [Attribute()] ResourceName layout;
			ProgressBarWidget batteryBar;

   Widget root ;
	void Deploy(){
	
	  root = GetGame().GetWorkspace().CreateWidgets(layout);

  
  
		 batteryBar= ProgressBarWidget.Cast(	root.FindAnyWidget("BatteryBar"));

	
	}
	void Disconnected(){
		if(root)
		delete root;
	}
	void EnergyUsed(float energy)
	{
		currentBattery = Math.Clamp(currentBattery-(energy*batteryConsumptionPerForceUnite),0,maxBattery);
	
		Rpc(RPC_CurrentBattery,currentBattery);
		//Print(currentBattery);

	}
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	void RPC_CurrentBattery(float batteryVal)
	{
		currentBattery = batteryVal;
		if(batteryBar)
		batteryBar.SetCurrent(currentBattery);

	}
	
	bool isDead(){return currentBattery<=0;}
	override void EOnPostFrame(IEntity owner, float timeSlice)
	{
		
	}
	void OnDelete(IEntity owner){
		if(root)
		delete root;
	
	}

	
}