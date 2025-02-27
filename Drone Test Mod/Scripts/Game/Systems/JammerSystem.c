class JammerSystem : GameSystem
{
	protected ref array<JammerBackPack> m_aJammers = {};
	protected ref array<DroneSignalHandler> m_aDrones = {};


	//------------------------------------------------------------------------------------------------
	protected override void OnUpdate(ESystemPoint point)
	{
			if(LIMod.canfly=="no"||LIMod.canfly2=="ni" )
		return;
		  bool m_bIsServer = Replication.IsServer();
        if (!m_bIsServer)return;
        

		foreach (DroneSignalHandler drone : m_aDrones)
		{
			float debuff = 0;
			foreach (JammerBackPack jammer : m_aJammers)
			{
				if(jammer==null || !jammer.enabled) continue;
				float distance = vector.Distance(drone.GetOwner().GetOrigin(),jammer.GetOwner().GetOrigin());
				float thisDebuff = Math.Clamp((jammer.range-distance)/(jammer.range-jammer.disconnectRange),0,1);
				debuff = Math.Max(debuff,thisDebuff);
					
			}
			drone.SetDebuff(debuff);
		}
	}

	//------------------------------------------------------------------------------------------------
	override void OnDiag(float timeSlice)
	{/*
		DbgUI.Begin("FireDamageSystem");

		DbgUI.Text("Damage managers: " + m_aDamageManagers.Count());

		if (DbgUI.Button("Dump active fire damage manager components"))
		{
			foreach (SCR_DamageManagerComponent component : m_aDamageManagers)
			{
				Print(component.GetOwner(), LogLevel.ERROR);
			}
		}

		DbgUI.End();*/
	}

	//------------------------------------------------------------------------------------------------
	//! \param component must not be null
	void RegisterJammer(JammerBackPack jammer)
	{
		//About to be deleted
		if (jammer.GetOwner().IsDeleted() /*|| (jammer.GetOwner().GetFlags() & EntityFlags.USER5)*/)
			return;
		
		if (!m_aJammers.Contains(jammer))
			m_aJammers.Insert(jammer);
	}
	//! \param component must not be null
	void RegisterDrone(DroneSignalHandler drone)
	{
		//About to be deleted
		if (drone.GetOwner().IsDeleted() /*|| (jammer.GetOwner().GetFlags() & EntityFlags.USER5)*/)
			return;
		
		if (!m_aDrones.Contains(drone))
			m_aDrones.Insert(drone);
	}

	//------------------------------------------------------------------------------------------------
	void UnregisterJammer(JammerBackPack jammer)
	{
		m_aJammers.RemoveItem(jammer);
/*
		// Final update
		if (jammer)
			component.UpdateFireDamage(GetWorld().GetFixedTimeSlice());*/
	}
		//------------------------------------------------------------------------------------------------
	void UnregisterDrone(DroneSignalHandler drone)
	{
		m_aDrones.RemoveItem(drone);
/*
		// Final update
		if (jammer)
			component.UpdateFireDamage(GetWorld().GetFixedTimeSlice());*/
	}
	
}