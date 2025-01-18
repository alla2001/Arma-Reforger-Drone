

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
	[Attribute("0.25")]
	float batteryConsumptionPerForceUnite;

	
	
	void EnergyUsed(float energy)
	{
		currentBattery = Math.Clamp(currentBattery-(energy*batteryConsumptionPerForceUnite),0,maxBattery);
		//Print(currentBattery);
	}
	bool isDead(){return currentBattery<=0;}
	override void EOnPostFrame(IEntity owner, float timeSlice)
	{
		
	}

	
}