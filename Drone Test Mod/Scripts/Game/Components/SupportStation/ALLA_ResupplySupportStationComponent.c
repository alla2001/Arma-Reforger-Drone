[ComponentEditorProps(category: "GameScripted/SupportStation", description: "")]
class ALLA_ResupplySupportStationComponentClass : SCR_ResupplySupportStationComponentClass
{
}

class ALLA_ResupplySupportStationComponent : SCR_ResupplySupportStationComponent
{
	
	//------------------------------------------------------------------------------------------------
	protected override int GetSupplyCostAction(IEntity actionOwner, IEntity actionUser, SCR_BaseUseSupportStationAction action)
	{
			return 999;
		if (!AreSuppliesEnabled())
			return 0;
		
		SCR_BaseResupplySupportStationAction resupplyAction = SCR_BaseResupplySupportStationAction.Cast(action);
		
		SCR_EntityCatalogEntry catalogEntry = m_EntityCatalogManager.GetEntryWithPrefabFromAnyCatalog(EEntityCatalogType.ITEM, resupplyAction.GetItemToResupply(), GetFaction());
		if (!catalogEntry)
		{
			Print("'SCR_ResupplySupportStationComponent' could not find SCR_EntityCatalogEntry for '" + resupplyAction.GetItemToResupply() + "' so will use fallback cost!", LogLevel.WARNING);
			return Math.ClampInt(m_iFallbackItemSupplyCost + m_iBaseSupplyCostOnUse, 0, int.MAX);
		}
		
		//~ Could not find arsenal data use fallback cost
		SCR_ArsenalItem arsenalData = SCR_ArsenalItem.Cast(catalogEntry.GetEntityDataOfType(SCR_ArsenalItem));
		if (!arsenalData)
		{
			Print("'SCR_ResupplySupportStationComponent' could not find SCR_ArsenalItem '" + resupplyAction.GetItemToResupply() + "' so will use fallback cost!", LogLevel.WARNING);
			return Math.ClampInt(m_iFallbackItemSupplyCost + m_iBaseSupplyCostOnUse, 0, int.MAX);
		}
		
		//return Math.ClampInt(arsenalData.GetSupplyCost(m_eSupplyCostType) + m_iBaseSupplyCostOnUse, 0, int.MAX);
	
	}
	
	
	
}
