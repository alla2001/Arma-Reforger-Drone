[ComponentEditorProps(category : "Tutorial/Component", description : "TODO")] class DroneControllerClass : ScriptComponentClass
{
}

class DroneController : ScriptComponent
{
    [Attribute()] float throttleForce; // Force applied for vertical movement
    [Attribute()] float moveSpeed;     // Speed for horizontal movement
    [Attribute()] float rotationSpeed; // Speed for yaw rotation
    [Attribute()] float tiltSpeed;     // Speed for drone tilting
    [Attribute()] float maxTiltAngle;  // Maximum tilt angle for DroneController
    [Attribute()] float groundRayDistance;
	 [Attribute()] float groundRayOffset;
    [Attribute()] int debugd;
 [Attribute()] float forceToExplode;
	[Attribute(params: "et")]
	protected ResourceName camPrefab;
	
    SCR_ManualCamera  camera;
    Physics rb;
    vector moveInput;
    float yawInput;
    float throttleInput;
    InputManager inputManager;
    [Attribute()] ResourceName layout;
    [Attribute()] string rtTextureName;
    [Attribute()] string rtName;
    [Attribute()] int m_iCameraIndex;
    [RplProp(condition: RplCondition.OwnerOnly)]bool deployed;
	[Attribute()] vector cameraSpawnPoint;
	
	 DroneSignalHandler droneSignalHandler;
	DroneBatteryHandler batteryHandler;
	ref array<DroneWingSpine> wings ;
	vector lastPos;
    override void OnPostInit(IEntity owner)
    {
        super.OnPostInit(GetOwner());
        rb = owner.GetPhysics();
        inputManager = GetGame().GetInputManager();
        SetEventMask(owner, EntityEvent.FRAME);
        SetEventMask(owner, EntityEvent.SIMULATE);
        SetEventMask(owner, EntityEvent.INIT);
        SetEventMask(owner, EntityEvent.CONTACT);
		if(debugd!=0){
		DiagMenu.SetValue(SCR_DebugMenuID.DEBUGUI_INPUT_MANAGER,debugd);
			  inputManager.SetDebug(debugd);
		}
	
        return;
    }
    void DeployDrone(IEntity drone, IEntity user)
    {
	 	Print("Deploying");
		if (!deployed)
            return;
		SoundComponent soundData = SoundComponent.Cast(GetOwner().FindComponent(SoundComponent));
			if (!soundData)
					soundData.SoundEvent("SOUND_FLY");
	
        bool m_bIsServer = Replication.IsServer();
        if (user != GetGame().GetPlayerController().GetControlledEntity())
        {
            if (!m_bIsServer)
                return;
            else
            {
                BaseRplComponent rplc = BaseRplComponent.Cast(drone.FindComponent(RplComponent));
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
		BaseRplComponent rplc = BaseRplComponent.Cast(drone.FindComponent(RplComponent));
		Print("RPC Auth :" + rplc.IsOwner(), LogLevel.ERROR);
       
        rb.EnableGravity(true); // Ensure gravity is enabled for realistic falling
        rb.SetActive(ActiveState.ALWAYS_ACTIVE);
        rb.ChangeSimulationState(SimulationState.SIMULATION);
        // Create layout
        /* Widget root = GetGame().GetWorkspace().CreateWidgets(layout);

         // We dont have required RT widgets, delete layout and terminate
         RTTextureWidget RTTexture = RTTextureWidget.Cast(root.FindAnyWidget(rtTextureName));
         RenderTargetWidget RTWidget = RenderTargetWidget.Cast(root.FindAnyWidget(rtName));
         // Set camera index of render target widget
         BaseWorld baseWorld = drone.GetWorld();

         RTWidget.SetWorld(baseWorld, m_iCameraIndex);
         if (!RTTexture || !RTWidget)
         {
             root.RemoveFromHierarchy();
         }*/
 		CreateCamera(drone);
        GetGame().GetCameraManager().SetCamera(camera);
			inputManager.AddActionListener("MouseRight",EActionTrigger.DOWN,TriggerExplode);
       
		lastPos=drone.GetOrigin();
		droneSignalHandler.Deploy(user,camera);
		 deployed = true;
		
    }
	CameraManager m_CameraManager;
    bool once = false;
	
    override void EOnInit(IEntity owner)
    {
			 wings ={};
		array<Managed> comps = new array<Managed>();
		FindComponentsInAllChildren(DroneWingSpine,owner,false,1,2,comps);
		foreach(Managed comp : comps){
		wings.Insert(DroneWingSpine.Cast(comp));
		}
		batteryHandler=DroneBatteryHandler.Cast(owner.FindComponent(DroneBatteryHandler));
		if(batteryHandler==null){
		Print("Missing Battery Handler On Drone !.", LogLevel.WARNING);
		}
		droneSignalHandler=DroneSignalHandler.Cast(owner.FindComponent(DroneSignalHandler));
		if(droneSignalHandler==null){
		Print("Missing Signal Handler On Drone !.", LogLevel.WARNING);
		}
	
    }
	protected void CreateCamera(IEntity parent)
	{
		// Spawn camera
		BaseWorld baseWorld = parent.GetWorld();

		// Spawn it close
		EntitySpawnParams params = new EntitySpawnParams();
		
		camera = SCR_ManualCamera.Cast(GetGame().SpawnEntityPrefab(Resource.Load(camPrefab), baseWorld, params));

	
		camera.SetOrigin(cameraSpawnPoint);
		camera.SetAngles(Vector(0,0,0));
		camera.SetCameraIndex(m_iCameraIndex);
		camera.SetVerticalFOV(85);
		camera.SetNearPlane(0.01);
		camera.SetFarPlane(750);
		camera.SetInputEnabled(true);
		
		baseWorld.SetCameraLensFlareSet(m_iCameraIndex, CameraLensFlareSetType.FirstPerson, string.Empty);

		// Set camera to hierarchy
		parent.AddChild(camera, -1, EAddChildFlags.AUTO_TRANSFORM);
	
		
	}
    override void EOnFrame(IEntity owner, float timeSlice)
    {
    
        // once =true;
        // Print("cam : " + 	GetGame().GetCameraManager().CurrentCamera ());
        if (!deployed)
            return;
        // Altitude control input

        throttleInput = inputManager.GetActionValue("CharacterFire");

        // Get inputs
        // Up (Jump) / Down (Fire3)
        moveInput = Vector(-inputManager.GetActionValue("CharacterRight"), 0, inputManager.GetActionValue("CharacterForward")); // Forward/Backward/Strafe
        yawInput = -inputManager.GetActionValue("CharacterLeanLeft") + inputManager.GetActionValue("CharacterLeanRight");       // Yaw (Mouse X)

        // Store last tilt angle for persistence
        vector lastTargetTilt;

        // Constrain tilt angles
        if (moveInput.Length() > 0) // Check if there is input
        {
            lastTargetTilt = Vector(
                Math.Clamp(-moveInput[2] * maxTiltAngle, -maxTiltAngle, maxTiltAngle), // Tilt forward/backward
                owner.GetYawPitchRoll()[1],                                            // Preserve yaw
                Math.Clamp(moveInput[0] * maxTiltAngle, -maxTiltAngle, maxTiltAngle)   // Tilt left/right
            );
            // owner.SetAngles(vector.Lerp(owner.GetAngles(), lastTargetTilt, tiltSpeed * timeSlice));
        }
        // Print("HEHE");
    }

    // vector forward = camMat[2];
    // 		vector up = camMat[1];
    // 		vector side = camMat[0];
    override void EOnSimulate(IEntity owner, float timeSlice)
    {
		
		
		
        if (!deployed)
            return;
		
		   BaseWorld baseWorld = owner.GetWorld();
		vector dir = lastPos- owner.GetOrigin();
		TraceParam param = MakeTraceParam(lastPos, owner.GetOrigin()+dir, TraceFlags.DEFAULT);
        float hit = baseWorld.TraceMove(param, null);
		owner.SetOrigin(vector.Lerp(lastPos,owner.GetOrigin(),hit));
        vector mat[4];
        owner.GetTransform(mat);
     
       param = MakeTraceParam(owner.GetOrigin() - mat[1] * groundRayOffset, owner.GetOrigin() - mat[1] * groundRayDistance, TraceFlags.DEFAULT);
         hit = baseWorld.TraceMove(param, null);

        // Print("Hit Down : " + hit);
        //  Get drone mass
        float mass = rb.GetMass();

        // Calculate gravitational force
        float gravityForce = mass * 9.81;

        // rb.ApplyForce((mat[1] * gravityForce));
        float mul = 0.5 + (1.0 - hit);
        // Apply vertical force
        vector upwardForce = ((mat[1] + (mat[2] * 0.1)) * throttleInput * throttleForce * mul);
        rb.ApplyForce(upwardForce);
		foreach(DroneWingSpine wing : wings){
		wing.SetSpinSpeed(throttleInput * throttleForce * mul);
		}
        // Apply horizontal force
        vector localMove = owner.VectorToParent(moveInput) * moveSpeed;
        // rb.ApplyForce(localMove);

        vector vInput = vector.Lerp(vInput, moveInput, tiltSpeed);
        // Apply yaw torque
        rb.ApplyTorque(mat[1] * yawInput * rotationSpeed);
        // Apply yaw torque
        rb.ApplyTorque(mat[0] * vInput[2] * rotationSpeed);
        // Apply yaw torque
        rb.ApplyTorque(mat[2] * vInput[0] * rotationSpeed);
        // Print( "Was Triggered : " + SCR_ExplosiveTriggerComponent.Cast(owner.FindComponent(SCR_ExplosiveTriggerComponent)).WasTriggered());
        //  Stabilize
        vector velocityDamping = -rb.GetVelocity() * 0.1;
        vector angularDamping = -rb.GetAngularVelocity() * 0.5;
        rb.ApplyForce(velocityDamping);
        rb.ApplyTorque(angularDamping);
		lastPos=owner.GetOrigin();
		
		batteryHandler.EnergyUsed( throttleInput +yawInput+ vInput[2]+ vInput[0]);
		if(batteryHandler.isDead())TriggerExplode();
    }
    override void EOnContact(IEntity owner, IEntity other, Contact contact)
    {

        // Print("Hit");
        if (!deployed)
            return;
        if (contact.Impulse < forceToExplode)
            return;
    	Rpc(Explode);
    }
	void TriggerExplode()
	{
	Rpc(Explode);
	}
	bool isDeployed(){return deployed;}
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void Explode(){
	    SCR_ExplosiveTriggerComponent m_Trigger = SCR_ExplosiveTriggerComponent.Cast(GetOwner().FindComponent(SCR_ExplosiveTriggerComponent));
        m_Trigger.SetLive();
        m_Trigger.UseTrigger();
	}
}
