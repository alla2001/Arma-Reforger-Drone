//------------------------------------------------------------------------------------------------
//! modded version for to be used with the inventory 2.0
class SCR_OpenStorageRankAction : SCR_OpenStorageAction
{
	[Attribute("1", desc: "Affected rank", uiwidget: UIWidgets.SearchComboBox, enums: ParamEnumArray.FromEnum(SCR_ECharacterRank))]
	SCR_ECharacterRank RankNeeded;
	#ifndef DISABLE_INVENTORY
	//------------------------------------------------------------------------------------------------
	override protected void PerformActionInternal(SCR_InventoryStorageManagerComponent manager, IEntity pOwnerEntity, IEntity pUserEntity)
	{
		IEntity player = GetGame().GetPlayerController().GetControlledEntity();
		if (pUserEntity != player|| SCR_CharacterRankComponent.GetCharacterRank(pUserEntity)<RankNeeded){
			UniversalInventoryStorageComponentDrone.Cast(GetOwner().FindComponent(UniversalInventoryStorageComponentDrone)).IsRank=false;
		return;
		}
		auto vicinity = CharacterVicinityComponent.Cast(pUserEntity .FindComponent(CharacterVicinityComponent));
		if (!vicinity)
			return;
		UniversalInventoryStorageComponentDrone.Cast(GetOwner().FindComponent(UniversalInventoryStorageComponentDrone)).IsRank=true;
		manager.SetStorageToOpen(pOwnerEntity);
		manager.OpenInventory();
	}
		//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{

		if (SCR_CharacterRankComponent.GetCharacterRank(user)<RankNeeded) {

			SetCannotPerformReason("rank not Enough");

			return false;

		}

		return super.CanBePerformedScript(user);
	}
	

	#endif


}
