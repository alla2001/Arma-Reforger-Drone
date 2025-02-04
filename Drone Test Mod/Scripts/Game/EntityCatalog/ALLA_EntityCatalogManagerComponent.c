//! Manager for non-faction specific entity catalogs as well as getters for faction specific catalogs


modded class SCR_EntityCatalogManagerComponent : SCR_BaseGameModeComponent
{
	override SCR_EntityCatalogEntry GetEntryWithPrefabFromFactionCatalog(EEntityCatalogType catalogType, ResourceName prefabToFind, notnull SCR_Faction faction)
	{
		//~ No prefab given
		if (SCR_StringHelper.IsEmptyOrWhiteSpace(prefabToFind))
			return null;

		//~ Get catalog
		SCR_EntityCatalog catalog = GetFactionEntityCatalogOfType(catalogType, faction);
		if (!catalog)
		{
			Game game = GetGame();
			if (game==null)return null;
			World world = game.GetWorld();
			if (world==null)return null;
					RankCatalogSystem RankCat= RankCatalogSystem.Cast(world.FindSystem(RankCatalogSystem));
			if (!RankCat)
			return null;
			return RankCat.GetEntryWithPrefab(prefabToFind);
		}
		SCR_EntityCatalogEntry entry=catalog.GetEntryWithPrefab(prefabToFind);

		if (entry==null){
			Game game = GetGame();
			if (game==null)return null;
			World world = game.GetWorld();
			if (world==null)return null;
					RankCatalogSystem RankCat= RankCatalogSystem.Cast(world.FindSystem(RankCatalogSystem));
			if (!RankCat)
			return null;
			return RankCat.GetEntryWithPrefab(prefabToFind);
		}

		//~ Try and find prefab
		return entry;
	}
}
