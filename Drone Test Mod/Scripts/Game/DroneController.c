[ComponentEditorProps(category : "Tutorial/Component", description : "TODO")] class DroneControllerClass : ScriptComponentClass
{
}

class DroneController : ScriptComponent
{
	[Attribute()] float throttleForce; // Force applied for vertical movement
	[Attribute()] float moveSpeed; // Speed for horizontal movement
	[Attribute()] float rotationSpeed; // Speed for yaw rotation
	[Attribute()] float tiltSpeed; // Speed for drone tilting
	[Attribute()] float maxTiltAngle; // Maximum tilt angle for DroneController
	[Attribute()] float groundRayDistance;
	[Attribute()] float groundRayOffset;
	[Attribute()] float fakeGravityForce;
	[Attribute()] int debugd;
	[Attribute()] float forceToExplode;
	[Attribute()] float interpolationThreshold;
	[Attribute()] float interpolationSpeed;
	[Attribute()] float speedCap;
	[Attribute(params : "et")] protected ResourceName camPrefab;

	SCR_ManualCamera camera;
	Physics rb;
	vector moveInput;
	float yawInput;
	float throttleInput;
	InputManager inputManager;

	Widget root;
	[Attribute()] int m_iCameraIndex;
	[RplProp(onRplName : "OnDeployed")] bool deployed;
	[RplProp()] bool armed;
		[RplProp()] bool autoHoverOn;

	[Attribute(defvalue : "0 0 0", uiwidget : UIWidgets.Coords, params : "inf inf 0 purpose=coords space=entity anglesVar=cameraSpawnRotation")] vector cameraSpawnPoint;
	[Attribute(defvalue : "0 0 0", uiwidget : UIWidgets.Coords, params : "inf inf 0 purpose=angles space=entity coordsVar=cameraSpawnPoint")] vector cameraSpawnRotation;
	[Attribute()] ResourceName layout;
		[Attribute()] bool canAutoHover;

	[Attribute()] bool canZoom;
		[Attribute()] float zoomMin;
	[Attribute()] float zoomMax;
	[Attribute()] float altitudeStabilizationStrength;
	[Attribute()] float maxStabilizationForce;
	[Attribute()] float driftStabilizationStrength;
		[Attribute()] float rotationStabilizationStrength;
	IEntity playerUser;
	DroneSignalHandler droneSignalHandler;
	DroneBatteryHandler batteryHandler;
	ref array<DroneWingSpine> wings;
	vector lastPos;
	NwkMovementComponent networkedMovment;
	SCR_PlaceableInventoryItemComponent placable;
	BaseRplComponent rplc;
	float mass = 1;
	float sfxVolume = 0;
	float dialgoVolume;
	[RplProp()]
	bool phyiscal;
	//! The render target texture found within our layout
	protected RTTextureWidget m_wRenderTargetTextureWidget;
	static bool inDrone;
	//! The render target found within our layout
	protected RenderTargetWidget m_wRenderTargetWidget;

	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(GetOwner());

		inputManager = GetGame().GetInputManager();
		SetEventMask(owner, EntityEvent.FRAME);
		SetEventMask(owner, EntityEvent.SIMULATE);
		SetEventMask(owner, EntityEvent.INIT);
		SetEventMask(owner, EntityEvent.CONTACT);

		if (debugd != 0)
		{
			DiagMenu.SetValue(SCR_DebugMenuID.DEBUGUI_INPUT_MANAGER, debugd);
			inputManager.SetDebug(debugd);
		}

		rb = owner.GetPhysics();
		if (!rb)
			return;
		mass = rb.GetMass();
		rb.SetActive(ActiveState.INACTIVE);
		rb.ChangeSimulationState(SimulationState.NONE);
		phyiscal = false;
Replication.BumpMe();
		return;
	}

	void DeployDrone(IEntity drone, IEntity user)
	{

		if (droneSignalHandler.isjammed()) return;
		sfxVolume = 0;
		lastPos = GetOwner().GetOrigin();
		bool m_bIsServer = Replication.IsServer();
		if (m_bIsServer)
		{
			phyiscal = true;
			Replication.BumpMe();
			lastPos = drone.GetOrigin();

			SetDynamic(GetOwner());

			droneSignalHandler.SetControlingPlayer(user);
		}
		else
		{

			phyiscal = true;
		}

		playerUser = user;
		PlayerController playerC = GetGame().GetPlayerController();

		IEntity playerEnt = null;
		if (playerC)
			playerEnt = playerC.GetControlledEntity();


		// Create layout
		if (playerUser != playerEnt)
		{
			if (!m_bIsServer)
				return;
			else
			{
				SCR_ExplosiveTriggerComponent m_Trigger = SCR_ExplosiveTriggerComponent.Cast(GetOwner().FindComponent(SCR_ExplosiveTriggerComponent));

				m_Trigger.SetInstigator(Instigator.CreateInstigator(user));
				deployed = true;
				Replication.BumpMe();
				rplc = BaseRplComponent.Cast(drone.FindComponent(RplComponent));
				if (!rplc)
				{
					Print("This example requires that the entity has an RplComponent.", LogLevel.WARNING);
					return;
				}

				SCR_ChimeraCharacter chim = SCR_ChimeraCharacter.Cast(user);
				rplc.Give(Replication.FindOwner(Replication.FindId(chim)));
				Print("RPC Auth :" + rplc.IsOwner(), LogLevel.ERROR);
			}
			return;
		}
		deployed = true;
		inDrone=true;
		CreateCamera(drone);
		GetGame().GetCameraManager().SetCamera(camera);
		inputManager.ActivateContext("PlayerCameraContext", duration :int.MAX);
		inputManager.ActivateContext("HelicopterContext", duration : int.MAX);
		inputManager.ActivateAction("CollectiveIncrease", duration : int.MAX);
		inputManager.ActivateAction("CollectiveIncrease", duration : int.MAX);
		inputManager.ActivateAction("CollectiveDecrease", duration : int.MAX);
		inputManager.ActivateAction("CyclicRight", duration : int.MAX);
		inputManager.ActivateAction("CyclicLeft", duration : int.MAX);
		inputManager.ActivateAction("CyclicForward", duration : int.MAX);
		inputManager.ActivateAction("CyclicBack", duration : int.MAX);
		inputManager.ActivateAction("AntiTorqueLeft", duration : int.MAX);
		inputManager.ActivateAction("AntiTorqueRight", duration : int.MAX);
		inputManager.ActivateAction("Focus", int.MAX);
		inputManager.ActivateAction("HelicopterFire", int.MAX);

		inputManager.AddActionListener("JumpOut", EActionTrigger.DOWN, ClientDisconnect);
		inputManager.AddActionListener("AutohoverToggle", EActionTrigger.DOWN, ToggleAutoHover);
		if(!canZoom)
		inputManager.AddActionListener("HelicopterFire", EActionTrigger.DOWN, TriggerExplode);
		inputManager.AddActionListener("Focus", EActionTrigger.VALUE, Zoom);


		lastPos = drone.GetOrigin();
		droneSignalHandler.Deploy(user, camera);
		batteryHandler.Deploy();

		SCR_ExplosiveTriggerComponent m_Trigger = SCR_ExplosiveTriggerComponent.Cast(GetOwner().FindComponent(SCR_ExplosiveTriggerComponent));

		m_Trigger.SetInstigator(Instigator.CreateInstigator(user));
		sfxVolume = AudioSystem.GetMasterVolume(AudioSystem.SFX);

		if (sfxVolume != 0)
			AudioSystem.SetMasterVolume(AudioSystem.SFX, 0.05);
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Server)] void RPC_ArmDrone()
	{
		armed = true;
		Replication.BumpMe();
	}
		[RplRpc(RplChannel.Reliable, RplRcver.Server)] void RPC_DisarmeDrone()
	{

		armed = false;
		Replication.BumpMe();
	}

	void ArmDrone()
	{

		SoundComp soundData = SoundComp.Cast(GetOwner().FindComponent(SoundComp));

		if (soundData)
			soundData.SoundEvent("SOUND_CONNECT");
		Rpc(RPC_ArmDrone);
		bool m_bIsServer = Replication.IsServer();
		if (m_bIsServer)
		{

			// rb.SetActive(ActiveState.ALWAYS_ACTIVE);
			// rb.ChangeSimulationState(SimulationState.SIMULATION);
		}
	}
	vector desiredPosition;
	void ToggleAutoHover()
	{
		if (!canAutoHover)return;
	autoHoverOn = !autoHoverOn;
		desiredPosition = GetOwner().GetOrigin(); // Lock the current position
	}

	void DisarmeDrone()
	{

		SoundComp soundData = SoundComp.Cast(GetOwner().FindComponent(SoundComp));

		if (soundData)
			soundData.SoundEvent("SOUND_DISARM");
		Rpc(RPC_DisarmeDrone);
	}
	float currentFOV;
	void Zoom(float zoom)
	{
		if (!canZoom)return;
		if (!camera)return;
		//if(zoom!=0) Print(zoom);
		currentFOV = Math.Lerp(zoomMax, zoomMin, zoom);

	camera.SetVerticalFOV(currentFOV);
	}

	void PickUp()
	{
	
		currentTransform[0] = vector.Zero;
		currentTransform[1] == vector.Zero;
		currentTransform[2] == vector.Zero;
		currentTransform[3] == vector.Zero;
		SoundComp soundData = SoundComp.Cast(GetOwner().FindComponent(SoundComp));

		if (soundData)
			soundData.TerminateAll();

		foreach (DroneWingSpine wing : wings)
		{
			wing.ToggleMesh(false);
		}

		rb.SetActive(ActiveState.INACTIVE);
		rb.ChangeSimulationState(SimulationState.NONE);
		phyiscal = false;


		PlayerController playerC = GetGame().GetPlayerController();

		IEntity playerEnt = null;
		if (playerC)
			playerEnt = playerC.GetControlledEntity();

		// Create layout

		if (deployed &&playerUser == playerEnt){
		ClientDisconnect();
		}
		bool m_bIsServer = Replication.IsServer();
		if (m_bIsServer)
		{
		}
	}

	void SetStatic(IEntity owner)
	{
		return;
		mass = rb.GetMass();
		rb.Destroy();
		PhysicsGeom geom = PhysicsGeom.CreateBox(Vector(0.6, 0.2, 0.6));

		PhysicsGeomDef geoms[] = {PhysicsGeomDef("box", geom, "", 0xffffffff)};
		rb = Physics.CreateStaticEx(owner, geoms);

		rb.EnableGravity(false); // Ensure gravity is enabled for realistic falling
		rb.SetActive(ActiveState.INACTIVE);
		rb.ChangeSimulationState(SimulationState.SIMULATION);
		phyiscal = false;
		Replication.BumpMe();
	}

	void SetDynamic(IEntity owner)
	{

		rb.Destroy();
		PhysicsGeom geom = PhysicsGeom.CreateBox(Vector(0.85, 0.32, 0.85));
		PhysicsGeomDef geoms[] = {PhysicsGeomDef("box", geom, "", 0xffffffff)};
		vector center = 0.097 * vector.Up;
		Physics.CreateDynamicEx(owner, center, mass, geoms);

		rb = owner.GetPhysics();
		rb.EnableGravity(true); // Ensure gravity is enabled for realistic falling
		rb.SetActive(ActiveState.ALWAYS_ACTIVE);
		rb.ChangeSimulationState(SimulationState.SIMULATION);
		phyiscal = true;
		Replication.BumpMe();
	}
	void Drop(ChimeraCharacter placingCharacter, SCR_PlaceableInventoryItemComponent placedItemIIC)
	{

		if (placable != placedItemIIC)
			return;
		if (!rb)
			rb = GetOwner().GetPhysics();
		SoundComp soundData = SoundComp.Cast(GetOwner().FindComponent(SoundComp));

		if (soundData && deployed)
			soundData.SoundEvent("SOUND_DISARM");

		foreach (DroneWingSpine wing : wings)
		{
			wing.ToggleMesh(true);
		}
		vector mat[4];
		GetOwner().GetTransform(mat);
		vector pos = GetOwner().GetOrigin();
		GetOwner().SetOrigin(pos + mat[1].Normalized() * 0.22);
		// was only for not armed so u can drop a drone and u walk on it and have it explode if its armed
		if (!armed)
		{
			phyiscal = false;
			rb.SetActive(ActiveState.INACTIVE);
			rb.ChangeSimulationState(SimulationState.NONE);
		}
		else
		{
			rb.SetActive(ActiveState.INACTIVE);
			rb.ChangeSimulationState(SimulationState.NONE);
			phyiscal = true;
		}
		Replication.BumpMe();
		bool m_bIsServer = Replication.IsServer();
		if (m_bIsServer)
		{
		}
	}

	CameraManager m_CameraManager;
	bool once = false;

	override void EOnInit(IEntity owner)
	{

		rplc = BaseRplComponent.Cast(owner.FindComponent(RplComponent));

		wings = {};
		array<Managed> comps = new array<Managed>();
		FindComponentsInAllChildren(DroneWingSpine, owner, false, 1, 2, comps);
		foreach (Managed comp : comps)
		{
			wings.Insert(DroneWingSpine.Cast(comp));
		}
		batteryHandler = DroneBatteryHandler.Cast(owner.FindComponent(DroneBatteryHandler));
		if (batteryHandler == null)
		{
			Print("Missing Battery Handler On Drone !.", LogLevel.WARNING);
		}
		droneSignalHandler = DroneSignalHandler.Cast(owner.FindComponent(DroneSignalHandler));
		if (droneSignalHandler == null)
		{
			Print("Missing Signal Handler On Drone !.", LogLevel.WARNING);
		}
		placable = SCR_PlaceableInventoryItemComponent.Cast(owner.FindComponent(SCR_PlaceableInventoryItemComponent));
		placable.GetOnPlacementDoneInvoker().Insert(Drop);
		placable.m_OnParentSlotChangedInvoker.Insert(SlotChanged);
		if (!rb)
			rb = owner.GetPhysics();

		if (!rb)
			return;
		mass = rb.GetMass();
		rb.SetActive(ActiveState.INACTIVE);
		rb.ChangeSimulationState(SimulationState.NONE);
		phyiscal = false;
		Replication.BumpMe();
	}
	void SlotChanged(InventoryStorageSlot oldslot, InventoryStorageSlot newslot)
	{

		if (newslot == null)
			Drop(null, placable);
		else
			PickUp();
	}
	void OnDeployed()
	{

		SoundComp soundData = SoundComp.Cast(GetOwner().FindComponent(SoundComp));

		if (soundData)
			soundData.SoundEvent("SOUND_FLY");
	}

	void CreateCamera(IEntity parent)
	{
		// Spawn camera
		BaseWorld baseWorld = parent.GetWorld();

		// Spawn it close
		EntitySpawnParams params = new EntitySpawnParams();

		camera = SCR_ManualCamera.Cast(GetGame().SpawnEntityPrefab(Resource.Load(camPrefab), baseWorld, params));

		camera.SetOrigin(cameraSpawnPoint);
		camera.SetAngles(cameraSpawnRotation);
		camera.SetCameraIndex(m_iCameraIndex);
		camera.SetVerticalFOV(zoomMax);
		camera.SetNearPlane(0.01);
		camera.SetFarPlane(1500);
		camera.SetInputEnabled(true);
		currentFOV=zoomMax;
		baseWorld.SetCameraLensFlareSet(m_iCameraIndex, CameraLensFlareSetType.FirstPerson, string.Empty);

		// Set camera to hierarchy
		parent.AddChild(camera, -1, EAddChildFlags.AUTO_TRANSFORM);
		/*
			root = GetGame().GetWorkspace().CreateWidgets(layout);
			m_wRenderTargetTextureWidget = RTTextureWidget.Cast(root.FindAnyWidget("RTTexture0"));
		m_wRenderTargetWidget = RenderTargetWidget.Cast(root.FindAnyWidget("RenderTarget0"));
			// Set camera index of render target widget

			m_wRenderTargetWidget.SetWorld(baseWorld, m_iCameraIndex);
		if (!parent.IsDeleted())
				m_wRenderTargetTextureWidget.SetGUIWidget(parent, 0); */

	}

	override void EOnFrame(IEntity owner, float timeSlice)
	{

		bool m_bIsServer = Replication.IsServer();
		if (!m_bIsServer && phyiscal)
		{
			if (interpolationProgress < 1.0)
			{

				// Check if the difference is too large
				bool shouldSetDirectly = false;
				for (int i = 0; i < 4; i++)
				{
					if ((currentTransform[i] - targetTransform[i]).Length() > interpolationThreshold)
					{
						shouldSetDirectly = true;
						break;
					}
				}

				if (shouldSetDirectly)
				{
					// Set directly to the target transform
					for (int i = 0; i < 4; i++)
					{
						currentTransform[i] = targetTransform[i];
					}
					interpolationProgress = 1.0; // Mark as completed
				}
				else
				{
					// Increment the interpolation progress
					interpolationProgress = Math.Clamp(interpolationProgress + timeSlice * interpolationSpeed, 0.0, 1.0);

					// Interpolate each element of the matrix
					for (int i = 0; i < 4; i++)
					{
						currentTransform[i] = vector.Lerp(currentTransform[i], targetTransform[i], interpolationProgress);
					}
				}
				GetOwner().SetTransform(currentTransform);
				GetOwner().Update();
			}
		}

		SoundComp soundData = SoundComp.Cast(GetOwner().FindComponent(SoundComp));
		if (!deployed && soundData && soundData.IsPlaying())
		{
			soundData.TerminateAll();
		}
		if (rb == null)
			return;
		if (m_bIsServer && phyiscal)
		{
			BaseWorld baseWorld = owner.GetWorld();

			vector mat[4];
			owner.GetTransform(mat);
			TraceParam param = MakeTraceParam(owner.GetOrigin() - mat[1].Normalized() * groundRayOffset, owner.GetOrigin() - mat[1].Normalized() * groundRayDistance, TraceFlags.OCEAN);
			float hit = baseWorld.TraceMove(param, null);
			if (hit <= 0.0)
				TriggerExplode();
			vector dir = lastPos - owner.GetOrigin();
			param = MakeTraceParam(lastPos, owner.GetOrigin() + dir * 0.5, TraceFlags.WORLD);
			hit = baseWorld.TraceMove(param, null);
			if (hit != 1)
			{
				owner.SetOrigin(vector.Lerp(lastPos + dir * 0.5, owner.GetOrigin(), hit) + dir * 0.5);
				owner.Update();
			}

			lastPos = owner.GetOrigin() + dir * 0.5;
		}

		if (m_bIsServer && phyiscal)
		{

			vector mat[4];
			owner.GetTransform(mat);
			Rpc(UpdateTransform, mat);
		}
		// once =true;
		// Print("cam : " + 	GetGame().GetCameraManager().CurrentCamera ());
		if (!deployed)
			return;
		// Altitude control input
		if (soundData && !soundData.IsPlaying())
		{
			soundData.SoundEvent("SOUND_FLY");
		}
		PlayerController playerC = GetGame().GetPlayerController();

		IEntity playerEnt = null;
		if (playerC)
			playerEnt = playerC.GetControlledEntity();

		if (!m_bIsServer && playerUser != playerEnt)
		{
			return;
		}

		if (playerUser != playerEnt)
		{
			return;
		}

		throttleInput += inputManager.GetActionValue("CollectiveIncrease")*timeSlice* 5;
		throttleInput -= inputManager.GetActionValue("CollectiveDecrease")*timeSlice* 5;
		throttleInput = Math.Lerp(throttleInput, 0, timeSlice* 9);
		throttleInput=Math.Clamp(throttleInput, -1, 1);
		// Get inputs
		// Up (Jump) / Down (Fire3)
		moveInput = Vector(inputManager.GetActionValue("CyclicLeft") - inputManager.GetActionValue("CyclicRight"), 0, inputManager.GetActionValue("CyclicForward") - inputManager.GetActionValue("CyclicBack")); // Forward/Backward/Strafe
		yawInput = -inputManager.GetActionValue("AntiTorqueLeft") + inputManager.GetActionValue("AntiTorqueRight"); // Yaw (Mouse X)

		moveInput = vector.Lerp(moveInput, vector.Zero, timeSlice* 9);
		// Store last tilt angle for persistence
		vector lastTargetTilt;

		// Constrain tilt angles
		if (moveInput.Length() > 0) // Check if there is input
		{
			lastTargetTilt = Vector(
				Math.Clamp(-moveInput[2] * maxTiltAngle, -maxTiltAngle, maxTiltAngle), // Tilt forward/backward
				owner.GetYawPitchRoll()[1], // Preserve yaw
				Math.Clamp(moveInput[0] * maxTiltAngle, -maxTiltAngle, maxTiltAngle) // Tilt left/right
			);
			// owner.SetAngles(vector.Lerp(owner.GetAngles(), lastTargetTilt, tiltSpeed * timeSlice));
		}

		Rpc(RPC_SendInputs, moveInput, yawInput, throttleInput);

		// Print("HEHE");
	}
	[RplRpc(RplChannel.Unreliable, RplRcver.Server)] void RPC_SendInputs(vector r_moveInput,
																		float r_yawInput,
																		float r_throttleInput)
	{
		moveInput = r_moveInput;
		throttleInput = r_throttleInput;
		yawInput = r_yawInput;
	}
	// vector forward = camMat[2];
	// 		vector up = camMat[1];
	// 		vector side = camMat[0];
	override void EOnSimulate(IEntity owner, float timeSlice)
	{

		bool m_bIsServer = Replication.IsServer();

		if (!m_bIsServer)
		{
			return;
		}

		if (!deployed)
			return;

		BaseWorld baseWorld = owner.GetWorld();

		vector mat[4];
		owner.GetTransform(mat);

		TraceParam param = MakeTraceParam(owner.GetOrigin() - mat[1] * groundRayOffset, owner.GetOrigin() - mat[1] * groundRayDistance, TraceFlags.OCEAN | TraceFlags.WORLD);
		float hit = baseWorld.TraceMove(param, null);
		param = MakeTraceParam(owner.GetOrigin() + mat[1] * groundRayOffset, owner.GetOrigin() + mat[1] * groundRayDistance, TraceFlags.OCEAN | TraceFlags.WORLD);
		float hit1 = baseWorld.TraceMove(param, null);

		// Print("Hit Down : " + hit);
		//  Get drone mass
		float mass = rb.GetMass();

		// Calculate gravitational force
		float gravityForce = mass * 9.81 * fakeGravityForce * timeSlice * hit;

		rb.ApplyForce(-(mat[1].Normalized() * gravityForce));
		float mul = 1.2 +Math.Pow( (1.0 - hit),3)* 2 +Math.Pow( (1.0 - hit1),3)* 0.3;
		// Apply vertical force
		vector upwardForce = ((mat[1].Normalized() + (mat[2].Normalized() * 0.1)) * throttleInput * throttleForce * mul * timeSlice);
		rb.ApplyForce(upwardForce);

		// Apply horizontal force
		vector localMove = owner.VectorToParent(moveInput) * moveSpeed;
		// rb.ApplyForce(localMove);

		vector vInput = vector.Lerp(vInput, moveInput, tiltSpeed);
		// Apply yaw torque
		rb.ApplyTorque(mat[1].Normalized() * yawInput * timeSlice* 2 * rotationSpeed);
		// Apply yaw torque
		rb.ApplyTorque(mat[0].Normalized() * vInput[2] * timeSlice * rotationSpeed);
		// Apply yaw torque
		rb.ApplyTorque(mat[2].Normalized() * vInput[0] * timeSlice * rotationSpeed);
		// Print( "Was Triggered : " + SCR_ExplosiveTriggerComponent.Cast(owner.FindComponent(SCR_ExplosiveTriggerComponent)).WasTriggered());
		//  Stabilize
		vector velocityDamping = -rb.GetVelocity() * 0;
		vector angularDamping = -rb.GetAngularVelocity() * 0.5;
		rb.ApplyForce(velocityDamping * timeSlice);
		rb.ApplyTorque(angularDamping * timeSlice);

		if (vInput[2] == 0 && vInput[0] == 0)
		{
			rb.ApplyTorque(angularDamping * timeSlice * 45);
		}

		rb.SetVelocity(rb.GetVelocity().Normalized()* Math.Min(speedCap, rb.GetVelocity().Length()));
		if (autoHoverOn && false){


			vector position = owner.GetOrigin();
			vector velocity = rb.GetVelocity();

			// --- Altitude Stabilization (Throttle Up/Down) ---
			float altitudeError = desiredPosition[1] - position[1]; // Compare current altitude to desired
			float verticalSpeed = velocity[1];

			// Throttle force proportional to altitude error and vertical speed
			float hoverthrottleForce = altitudeError * altitudeStabilizationStrength - verticalSpeed * 2.0;
			hoverthrottleForce = Math.Clamp(hoverthrottleForce, -throttleForce, throttleForce);
			rb.ApplyForce(vector.Up * hoverthrottleForce * timeSlice);

			// --- Drift Stabilization (Roll and Pitch) ---
			vector horizontalVelocity = Vector(velocity[0], 0, velocity[2]); // Only horizontal movement
			vector driftCorrectionForce = -horizontalVelocity * driftStabilizationStrength;

			// Determine desired roll/pitch angles based on drift correction
			float desiredPitch = driftCorrectionForce[2] / hoverthrottleForce; // Forward/backward tilt
			float desiredRoll = -driftCorrectionForce[0] / hoverthrottleForce; // Left/right tilt

			// Apply pitch and roll torques
			rb.ApplyTorque(Vector(desiredRoll * 10, 0, desiredPitch * 10) * timeSlice);

			// --- Rotation Stabilization (Yaw) ---
			vector currentAngularVelocity = rb.GetAngularVelocity();
			vector desiredForward = vector.Forward; // Lock to default forward orientation
			vector currentForward = owner.GetTransformAxis(2); // Get current forward vector

			// Calculate yaw error (difference between desired and current orientation)
			float yawError = vector.Dot(currentForward* desiredForward, vector.Up); // Sign of yaw error
			float yawCorrectionTorque = yawError * 10 - currentAngularVelocity[1] * 2.0; // Add damping
			rb.ApplyTorque(vector.Up * yawCorrectionTorque * timeSlice);
		}
		foreach (DroneWingSpine wing : wings)
		{
			wing.SetSpinSpeed((throttleInput + vInput[2] + vInput[0] + yawInput) * throttleForce * mul * timeSlice);
		}

		batteryHandler.EnergyUsed(Math.AbsFloat(throttleInput) + Math.AbsFloat(yawInput) + Math.AbsFloat(vInput[2]) + Math.AbsFloat(vInput[0]));
		if (batteryHandler.isDead())
			ClientDisconnect();

		if (droneSignalHandler.isOutOfRange())
			ClientDisconnect();
	}
	[RplRpc(RplChannel.Unreliable, RplRcver.Broadcast)] void UpdateTransform(vector tranMat[4])
	{
		/* if (!tranMat || tranMat.size() != 4) {
			Print("Invalid transformation matrix provided.", LogLevel.Error);
			return;
		}*/

		// Save the target transformation matrix for interpolation
		if (currentTransform[0] == vector.Zero && currentTransform[1] == vector.Zero && currentTransform[2] == vector.Zero && currentTransform[3] == vector.Zero)
			currentTransform = targetTransform;
		targetTransform = tranMat;
		interpolationProgress = 0.0; // Reset interpolation progress
	}

	// Interpolation variables
	vector currentTransform[4];
	vector targetTransform[4];

	float interpolationProgress = 1.0; // Completed when 1.0


	void ClientDisconnect()
	{
		Rpc(RPCClientDisconnect);
	}
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	void RPCClientDisconnect()
	{
		ClearConnection();
		Rpc(RPC_DisconnectServer);
	}
	void ClearConnection(){

	if (!deployed)
			return;
		inputManager.ActivateContext("PlayerCameraContext", duration :0);
		inputManager.ActivateContext("HelicopterContext", duration : 0);
		inputManager.ActivateAction("CollectiveIncrease", duration : 0);
		inputManager.ActivateAction("CollectiveIncrease", duration : 0);
		inputManager.ActivateAction("CollectiveDecrease", duration : 0);
		inputManager.ActivateAction("CyclicRight", duration :0);
		inputManager.ActivateAction("CyclicLeft", duration : 0);
		inputManager.ActivateAction("CyclicForward", duration : 0);
		inputManager.ActivateAction("CyclicBack", duration : 0);
		inputManager.ActivateAction("AntiTorqueLeft", duration : 0);
		inputManager.ActivateAction("AntiTorqueRight", duration : 0);
		inputManager.ActivateAction("FocusAnalog", duration : 0);
		inputManager.ActivateAction("HelicopterFire", duration : 0);

				inputManager.RemoveActionListener("JumpOut", EActionTrigger.DOWN, ClientDisconnect);
		inputManager.RemoveActionListener("AutohoverToggle", EActionTrigger.DOWN, ToggleAutoHover);
		inputManager.RemoveActionListener("HelicopterFire", EActionTrigger.DOWN, TriggerExplode);
		inputManager.RemoveActionListener("FocusAnalog", EActionTrigger.VALUE, Zoom);
		droneSignalHandler.Disconnected();
		batteryHandler.Disconnected();
		playerUser = null;
		if (sfxVolume != 0)
			AudioSystem.SetMasterVolume(AudioSystem.SFX, sfxVolume);
		delete camera;
		/*if (m_wRenderTargetTextureWidget && GetOwner() && !GetOwner().IsDeleted())
			m_wRenderTargetTextureWidget.SetGUIWidget(GetOwner(), -1); */
		if (root)
		delete root;
		inDrone=false;
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Server)] void RPC_DisconnectServer()
	{
		rplc.Give(RplIdentity.Local());
		playerUser = null;
		deployed = false;
		inDrone=false;
		Replication.BumpMe();
	}

	override void EOnContact(IEntity owner, IEntity other, Contact contact)
	{
		if (contact.Impulse > forceToExplode * 10)
		{
			TriggerExplode();
			return;
		}

		if (!armed)
			return;
		if (contact.Impulse < forceToExplode)
			return;

		TriggerExplode();
	}
	void TriggerExplode()
	{
		ClearConnection();
			Rpc(RPC_ServerExplode);

	}

	override void OnDelete(IEntity owner)
	{
		PlayerController playerC = GetGame().GetPlayerController();

		IEntity playerEnt = null;
		if (playerC)
			playerEnt = playerC.GetControlledEntity();
		if (playerUser == playerEnt)
		{
			ClearConnection();
			AudioSystem.SetMasterVolume(AudioSystem.SFX, 1);
		}
		if (root)
		delete root;
		sfxVolume = 0;
		bool m_bIsServer = Replication.IsServer();
		if (!m_bIsServer)
		{

			return;
		}
		//Rpc(RPC_ClientDisconnect);

	}

	bool isDeployed() { return deployed; }
	bool isArmed() { return armed; }

	[RplRpc(RplChannel.Reliable, RplRcver.Server)] void RPC_ServerExplode()
	{
		SCR_ExplosiveTriggerComponent m_Trigger = SCR_ExplosiveTriggerComponent.Cast(GetOwner().FindComponent(SCR_ExplosiveTriggerComponent));
		m_Trigger.SetLive();
		m_Trigger.UseTrigger();
	}
}
