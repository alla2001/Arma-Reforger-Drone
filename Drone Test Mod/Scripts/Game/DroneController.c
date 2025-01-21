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
	 [Attribute()] float fakeGravityForce;
    [Attribute()] int debugd;
    [Attribute()] float forceToExplode;
    [Attribute(params : "et")] protected ResourceName camPrefab;

    SCR_ManualCamera camera;
    Physics rb;
    vector moveInput;
    float yawInput;
    float throttleInput;
    InputManager inputManager;

    [Attribute()] int m_iCameraIndex;
    [RplProp(onRplName : "OnDeployed")] bool deployed;
    [RplProp(condition : RplCondition.NoOwner)] bool armed;
   [Attribute(defvalue: "0 0 0", uiwidget: UIWidgets.Coords, params: "inf inf 0 purpose=coords space=entity anglesVar=cameraSpawnRotation")] vector cameraSpawnPoint;
	   [Attribute(defvalue: "0 0 0", uiwidget: UIWidgets.Coords, params: "inf inf 0 purpose=angles space=entity coordsVar=cameraSpawnPoint")] vector cameraSpawnRotation;
    
	IEntity playerUser;
    DroneSignalHandler droneSignalHandler;
    DroneBatteryHandler batteryHandler;
    ref array<DroneWingSpine> wings;
    vector lastPos;
    NwkMovementComponent networkedMovment;
SCR_PlaceableInventoryItemComponent placable;
    BaseRplComponent rplc;
	
	float sfxVolume=0;
	float dialgoVolume;
    override void OnPostInit(IEntity owner)
    {
        super.OnPostInit(GetOwner());
        rb = owner.GetPhysics();
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

        return;
    }

    void DeployDrone(IEntity drone, IEntity user)
    {
      

        bool m_bIsServer = Replication.IsServer();
		 if (m_bIsServer){




        rb.EnableGravity(true); // Ensure gravity is enabled for realistic falling
        rb.SetActive(ActiveState.ALWAYS_ACTIVE);
        rb.ChangeSimulationState(SimulationState.SIMULATION);
			   droneSignalHandler.SetControlingPlayer(user);
		}
		 playerUser = user;
        // Create layout
        if (user != GetGame().GetPlayerController().GetControlledEntity())
        {
            if (!m_bIsServer)
                return;
            else
            {	SCR_ExplosiveTriggerComponent m_Trigger = SCR_ExplosiveTriggerComponent.Cast(GetOwner().FindComponent(SCR_ExplosiveTriggerComponent));

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

        CreateCamera(drone);
        GetGame().GetCameraManager().SetCamera(camera);

        inputManager.ActivateContext("HelicopterContext", duration : int.MAX);
        inputManager.ActivateAction("CollectiveIncrease", duration : int.MAX);
		    inputManager.ActivateAction("CollectiveIncrease", duration : int.MAX);
		    inputManager.ActivateAction("CollectiveDecrease", duration : int.MAX);
		inputManager.ActivateAction("CyclicRight", duration : int.MAX);
		inputManager.ActivateAction("CyclicLeft", duration : int.MAX);
		inputManager.ActivateAction("CyclicForward", duration : int.MAX);
		inputManager.ActivateAction("CyclicBack", duration : int.MAX);
		inputManager.ActivateAction("AntiTorqueLeft", duration :int.MAX);
		inputManager.ActivateAction("AntiTorqueRight", duration : int.MAX);
    


        inputManager.AddActionListener("JumpOut", EActionTrigger.DOWN, Disconnect);
        inputManager.AddActionListener("CharacterFire", EActionTrigger.DOWN, TriggerExplode);
        lastPos = drone.GetOrigin();
        droneSignalHandler.Deploy(user, camera);
        batteryHandler.Deploy();
		
		SCR_ExplosiveTriggerComponent m_Trigger = SCR_ExplosiveTriggerComponent.Cast(GetOwner().FindComponent(SCR_ExplosiveTriggerComponent));
	
		
		m_Trigger.SetInstigator(Instigator.CreateInstigator(user));
		sfxVolume=AudioSystem.GetMasterVolume(AudioSystem.SFX);
	
		if(sfxVolume!=0)
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
    }

    void DisarmeDrone()
    {

        SoundComp soundData = SoundComp.Cast(GetOwner().FindComponent(SoundComp));

        if (soundData)
            soundData.SoundEvent("SOUND_DISARM");
        Rpc(RPC_DisarmeDrone);
    }
		
	
	
    void PickUp()
    {

        SoundComp soundData = SoundComp.Cast(GetOwner().FindComponent(SoundComp));

        if (soundData)
            soundData.TerminateAll();
		
		foreach (DroneWingSpine wing : wings)
        {
            wing.ToggleMesh(false);
        }
      Disconnect();
    }

    void Drop(ChimeraCharacter placingCharacter, SCR_PlaceableInventoryItemComponent placedItemIIC)
    {
if(placable!=placedItemIIC)return;
        SoundComp soundData = SoundComp.Cast(GetOwner().FindComponent(SoundComp));

        if (soundData &&deployed)
            soundData.SoundEvent("SOUND_DISARM");
		
		foreach (DroneWingSpine wing : wings)
        {
            wing.ToggleMesh(true);
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
			placable=	SCR_PlaceableInventoryItemComponent.Cast( owner.FindComponent(SCR_PlaceableInventoryItemComponent));
  		placable.GetOnPlacementDoneInvoker().Insert(Drop);
		placable.m_OnParentSlotChangedInvoker.Insert(SlotChanged);
    }
	void SlotChanged(InventoryStorageSlot oldslot, InventoryStorageSlot newslot){
	
	if(newslot==null)
	Drop(null,null);
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
		     SoundComp soundData = SoundComp.Cast(GetOwner().FindComponent(SoundComp));
		  if (!deployed&&soundData && soundData.IsPlaying())
        {
            soundData.TerminateAll();
        }
		 bool m_bIsServer = Replication.IsServer();

        // once =true;
        // Print("cam : " + 	GetGame().GetCameraManager().CurrentCamera ());
        if (!deployed)
            return;
	     // Altitude control input
	    if (soundData && !soundData.IsPlaying())
        {
            soundData.SoundEvent("SOUND_FLY");
        }
		PlayerController playerC =  	GetGame().GetPlayerController();
	
		IEntity playerEnt =null;
		if(playerC)
		playerEnt = playerC.GetControlledEntity();
;
   		if (!m_bIsServer&& playerUser !=playerEnt)
        {
			return;
		}
		if (m_bIsServer)
        {
			BaseWorld baseWorld = owner.GetWorld();

        vector mat[4];
        owner.GetTransform(mat);
		 TraceParam     param = MakeTraceParam(owner.GetOrigin() - mat[1] * groundRayOffset, owner.GetOrigin() - mat[1] * groundRayDistance,  TraceFlags.OCEAN);
		float hit = baseWorld.TraceMove(param, null);
		if(hit<=0.01) TriggerExplode();
		  vector dir = lastPos - owner.GetOrigin();
        param = MakeTraceParam(lastPos, owner.GetOrigin()+dir*0.5,  TraceFlags.WORLD);
        hit = baseWorld.TraceMove(param, null);
		if(hit!=1){
		      owner.SetOrigin(vector.Lerp(lastPos, owner.GetOrigin(), hit)+dir*0.5);
					owner.Update();
		}
  
        lastPos = owner.GetOrigin()+dir*0.5;

		}
	
			
        		
    	if (playerUser != playerEnt)
        {
			return;
		}

 
        throttleInput = inputManager.GetActionValue("CollectiveIncrease")- inputManager.GetActionValue("CollectiveDecrease");

        // Get inputs
        // Up (Jump) / Down (Fire3)
        moveInput = Vector(inputManager.GetActionValue("CyclicLeft")-inputManager.GetActionValue("CyclicRight"), 0, inputManager.GetActionValue("CyclicForward")-inputManager.GetActionValue("CyclicBack")); // Forward/Backward/Strafe
        yawInput = -inputManager.GetActionValue("AntiTorqueLeft") + inputManager.GetActionValue("AntiTorqueRight");       // Yaw (Mouse X)

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
		
		Rpc(RPC_SendInputs,moveInput,yawInput,throttleInput);

        // Print("HEHE");
    }
	[RplRpc(RplChannel.Unreliable, RplRcver.Server)]
	void RPC_SendInputs( vector r_moveInput,
    float r_yawInput,
    float r_throttleInput)
	{
	 moveInput= r_moveInput;
 	throttleInput =r_throttleInput;
	yawInput =r_yawInput;
	
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
		
        TraceParam param = MakeTraceParam(owner.GetOrigin() - mat[1] * groundRayOffset, owner.GetOrigin() - mat[1] * groundRayDistance,  TraceFlags.OCEAN | TraceFlags.WORLD);
        float hit = baseWorld.TraceMove(param, null);

        // Print("Hit Down : " + hit);
        //  Get drone mass
        float mass = rb.GetMass();

        // Calculate gravitational force
        float gravityForce = mass * 9.81*fakeGravityForce*timeSlice*hit;

         rb.ApplyForce(-(mat[1] * gravityForce));
        float mul = 0.5 + (1.0 - hit);
        // Apply vertical force
        vector upwardForce = ((mat[1] + (mat[2] * 0.1)) * throttleInput * throttleForce * mul*timeSlice);
        rb.ApplyForce(upwardForce);

        // Apply horizontal force
        vector localMove = owner.VectorToParent(moveInput) * moveSpeed;
        // rb.ApplyForce(localMove);
	
        vector vInput = vector.Lerp(vInput, moveInput, tiltSpeed);
        // Apply yaw torque
        rb.ApplyTorque(mat[1] * yawInput*timeSlice * rotationSpeed);
        // Apply yaw torque
        rb.ApplyTorque(mat[0] * vInput[2] *timeSlice* rotationSpeed);
        // Apply yaw torque
        rb.ApplyTorque(mat[2] * vInput[0] *timeSlice* rotationSpeed);
        // Print( "Was Triggered : " + SCR_ExplosiveTriggerComponent.Cast(owner.FindComponent(SCR_ExplosiveTriggerComponent)).WasTriggered());
        //  Stabilize
        vector velocityDamping = -rb.GetVelocity() * 0.1;
        vector angularDamping = -rb.GetAngularVelocity() * 0.5;
        rb.ApplyForce(velocityDamping*timeSlice);
        rb.ApplyTorque(angularDamping*timeSlice);
		  foreach (DroneWingSpine wing : wings)
        {
            wing.SetSpinSpeed((throttleInput + vInput[2]+ vInput[0]+yawInput) * throttleForce * mul*timeSlice);
        }
  		mat[4];
        owner.GetTransform(mat);
 		Rpc(UpdateTransforme,mat);
		
		batteryHandler.EnergyUsed(Math.AbsFloat(throttleInput) + Math.AbsFloat(yawInput) + Math.AbsFloat(vInput[2]) + Math.AbsFloat(vInput[0]));
		  if (batteryHandler.isDead())
            Disconnect();
		
        if (droneSignalHandler.isOutOfRange())
            Disconnect();
		

		
      
    }
	 [RplRpc(RplChannel.Unreliable, RplRcver.Broadcast)]
	void UpdateTransforme(vector tranMat[4]){
	
		GetOwner().SetTransform(tranMat);
	}
    void Disconnect()
    {
        if (!deployed)
            return;
		
	 	deployed = false;
		 Replication.BumpMe();
   

        Rpc(RPC_Disconnect);
		  rplc.Give(Replication.FindOwner(Replication.FindId(GetGame().GetPlayerController().GetControlledEntity())));
  
        playerUser = null;
		
    }
    [RplRpc(RplChannel.Reliable, RplRcver.Owner)] void RPC_Disconnect()
    {
       	deployed = false;
		 Replication.BumpMe();
		     inputManager.RemoveActionListener("JumpOut", EActionTrigger.DOWN, Disconnect);
        inputManager.RemoveActionListener("CharacterFire", EActionTrigger.DOWN, TriggerExplode);
        droneSignalHandler.Disconnected();
        batteryHandler.Disconnected();
        inputManager.ActivateContext("HelicopterContext", duration : 0);
        inputManager.ActivateAction("JumpOut", duration : 0);
			if(sfxVolume!=0)
		AudioSystem.SetMasterVolume(AudioSystem.SFX, sfxVolume);
   delete camera;
		
       

        /*
         bool m_bIsServer = Replication.IsServer();

        if (playerUser != GetGame().GetPlayerController().GetControlledEntity())
        {
            if (!m_bIsServer)
                return;
            else
            {
                BaseRplComponent rplc = BaseRplComponent.Cast(GetOwner().FindComponent(RplComponent));
                if (!rplc)
                {
                    Print("This example requires that the entity has an RplComponent.", LogLevel.WARNING);
                    return;
                }
                SCR_ChimeraCharacter chim = SCR_ChimeraCharacter.Cast(playerUser);
                rplc.Give(Replication.FindOwner(Replication.FindId(chim)));
                Print("RPC Auth :" + rplc.IsOwner(), LogLevel.ERROR);
            }
            return;
        }
    */
    } override void EOnContact(IEntity owner, IEntity other, Contact contact)
    {

        if (!armed)
            return;
        if (contact.Impulse < forceToExplode)
            return;

        TriggerExplode();
    }
    void TriggerExplode()
    {
		
        Disconnect();
        Rpc(Explode);
    }
    override void OnDelete(IEntity owner)
    {
		bool m_bIsServer = Replication.IsServer();
		if (!m_bIsServer)
        {
			return;
		}
      		 Rpc(RPC_Disconnect);
      
    }
	
    bool isDeployed() { return deployed; }
    bool isArmed() { return armed; }
    [RplRpc(RplChannel.Reliable, RplRcver.Server)] void Explode()
    {
        SCR_ExplosiveTriggerComponent m_Trigger = SCR_ExplosiveTriggerComponent.Cast(GetOwner().FindComponent(SCR_ExplosiveTriggerComponent));
        m_Trigger.SetLive();
        m_Trigger.UseTrigger();
    }
}
