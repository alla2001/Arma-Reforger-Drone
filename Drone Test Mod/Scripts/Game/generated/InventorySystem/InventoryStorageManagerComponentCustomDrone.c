/*
===========================================
Do not modify, this script is generated
===========================================
*/

/*!
\addtogroup InventorySystem
\{
*/

class UniversalInventoryStorageComponentDroneClass: UniversalInventoryStorageComponentClass
{
	
}
class UniversalInventoryStorageComponentDrone: UniversalInventoryStorageComponent
{
	
	bool IsRank;
	override bool CanRemoveItem(IEntity item) { return IsRank; };
	override bool ShouldHideInVicinity(){return !IsRank;}
}

/*!
\}
*/
