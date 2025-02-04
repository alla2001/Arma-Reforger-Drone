//------------------------------------------------------------------------------------------------
//! Returns true if vehicle's brakes are at least at specified threshold
[BaseContainerProps()]
class SCR_InDroneCondition : SCR_AvailableActionCondition
{
	
	//------------------------------------------------------------------------------------------------
	//! Returns true when current gear matches the condition
	//! Returns opposite if m_bNegateCondition is enabled
	override bool IsAvailable(SCR_AvailableActionsConditionData data)
	{
		

		return DroneController.inDrone;
	}
};
