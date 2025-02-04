class ALLA_DroneArsenalClass : SCR_ArsenalComponentClass
{



}


class ALLA_DroneArsenal : SCR_ArsenalComponent
{

		override bool GetFilteredOverwriteArsenalItems(out notnull array<SCR_ArsenalItem> filteredArsenalItems, EArsenalItemDisplayType requiresDisplayType = -1)
	{
			filteredArsenalItems = m_OverwriteArsenalConfig.GetFilteredArsenalItems(m_eSupportedArsenalItemTypes, m_eSupportedArsenalItemModes, requiresDisplayType);
		foreach(SCR_ArsenalItem item : filteredArsenalItems)
		{
		Print("Cost:"+ item.GetSupplyCost(SCR_EArsenalSupplyCostType.DEFAULT));
		}
	
		return !filteredArsenalItems.IsEmpty();
	}

}