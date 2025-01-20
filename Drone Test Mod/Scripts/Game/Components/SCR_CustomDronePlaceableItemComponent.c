[EntityEditorProps(category: "GameScripted/Components", description: "")]
class SCR_CustomDronePlaceableItemComponentClass : SCR_ItemPlacementComponentClass
{
	
}

class  SCR_CustomDronePlaceableItemComponent : SCR_ItemPlacementComponent
{
	[Attribute()]
	float groundOffset;
override void UseXYZPlacement(IEntity owner, float maxPlacementDistance, vector cameraMat[4])
	{
		// Trace against terrain and entities to detect item placement position
		ChimeraCharacter character = ChimeraCharacter.Cast(SCR_PlayerController.GetLocalControlledEntity());
		if (!character)
			return;

		TraceParam param = new TraceParam();
		param.Start = character.EyePosition();
		param.End = param.Start + cameraMat[2] * maxPlacementDistance;
		param.Flags = TraceFlags.WORLD | TraceFlags.ENTS;
		param.Exclude = SCR_PlayerController.GetLocalControlledEntity();
		param.LayerMask = EPhysicsLayerPresets.Projectile;
		BaseWorld world = owner.GetWorld();
		float traceDistance = world.TraceMove(param, FilterCallback);
		m_PreviewEntity.GetTransform(m_vCurrentMat);
		m_vCurrentMat[3] = param.Start + ((param.End - param.Start) * traceDistance);
		vector up = param.TraceNorm;
		m_vCurrentMat[3] = m_vCurrentMat[3] + up * 0.01;

		IEntity tracedEntity = param.TraceEnt;

		if (traceDistance == 1) // Assume we didn't hit anything and snap item on the ground
		{
			// Trace against terrain and entities to detect new placement position
			TraceParam paramGround = new TraceParam();
			paramGround.Start = param.End + vector.Up;
			paramGround.End = paramGround.Start - vector.Up * 20;
			paramGround.Flags = TraceFlags.WORLD | TraceFlags.ENTS;
			paramGround.Exclude = SCR_PlayerController.GetLocalControlledEntity();
			paramGround.LayerMask = EPhysicsLayerPresets.Projectile;
			float traceGroundDistance = world.TraceMove(paramGround, FilterCallback);
			m_PreviewEntity.GetTransform(m_vCurrentMat);
			m_vCurrentMat[3] = paramGround.Start + ((paramGround.End - paramGround.Start) * traceGroundDistance) + vector.Up * 0.01+vector.Up *groundOffset; // adding 1 cm to avoid collision with object under

			if (traceGroundDistance < 1)
				up = paramGround.TraceNorm;

			tracedEntity = paramGround.TraceEnt;
		}

		SCR_EntityHelper.OrientUpToVector(up, m_vCurrentMat);
		vector right = up * cameraMat[0];
		vector forward = up * right;
		right.Normalize();
		forward.Normalize();
		m_vCurrentMat[0] = forward;
		m_vCurrentMat[2] = right;

#ifdef WORKBENCH
		ShapeFlags shapeFlags = ShapeFlags.ONCE | ShapeFlags.NOZBUFFER | ShapeFlags.TRANSP | ShapeFlags.DOUBLESIDE | ShapeFlags.NOOUTLINE;
		Shape.CreateArrow(m_vCurrentMat[3], m_vCurrentMat[3] + up * 0.5, 0.1, Color.BLUE, shapeFlags);
		Shape.CreateArrow(m_vCurrentMat[3], m_vCurrentMat[3] + right * 0.5, 0.1, Color.ORANGE, shapeFlags);
		Shape.CreateArrow(m_vCurrentMat[3], m_vCurrentMat[3] + forward * 0.5, 0.1, Color.GREEN, shapeFlags);
#endif

		m_PreviewEntity.SetTransform(m_vCurrentMat);
		m_PreviewEntity.Update();

		// Reject based on distance from character
		if (SCR_PlaceableItemComponent.GetDistanceFromCharacter(character, m_vCurrentMat[3], m_PlaceableItem.GetDistanceMeasurementMethod()) > maxPlacementDistance)
		{
			m_bCanPlace = false;
			return;
		}

		m_iTargetEntityNodeID = param.NodeIndex;
		m_aCamDeploymentPosition = cameraMat;
		m_aCamDeploymentPosition[3] = character.EyePosition();
		m_bCanPlace = ValidatePlacement(m_vCurrentMat[1], tracedEntity, world, SCR_PlayerController.GetLocalControlledEntity());

	}
	
}
