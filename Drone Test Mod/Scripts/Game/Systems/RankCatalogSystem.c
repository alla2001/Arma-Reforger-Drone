class RankCatalogSystem : GameSystem
{

	[Attribute()]
	protected ref array<ref SCR_EntityCatalogEntry> m_Entries;

	//------------------------------------------------------------------------------------------------
	protected override void OnUpdate(ESystemPoint point)
	{
		
	}

	 SCR_EntityCatalogEntry GetEntryWithPrefab(ResourceName prefabToFind)
	{
		
		foreach(SCR_EntityCatalogEntry entry : m_Entries)
		{
			if(entry.GetPrefab()==prefabToFind)
			return entry;
		
		}
		
		return null;
	}
	
}