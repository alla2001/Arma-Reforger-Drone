[ComponentEditorProps(category : "Tutorial/Component", description : "TODO")] class UAVDroneControllerClass : DroneControllerClass
{
}

class UAVDroneController : DroneController
{[Attribute()] float maxThrust;         // Maximum forward thrust (for propulsion)
[Attribute()] float liftCoefficient;   // Determines how much lift is generated
[Attribute()] float wingArea;          // Surface area of the wings (affects lift)
[Attribute()] float dragCoefficient;   // Determines air resistance (higher = more drag)
[Attribute()] float pitchTorque;       // Torque applied for pitch (nose up/down)
[Attribute()] float rollTorque;        // Torque applied for roll (banking left/right)
[Attribute()] float yawTorque;         // Torque applied for yaw (turning left/right)



	
	override void EOnSimulate(IEntity owner, float timeSlice)
	{

		bool m_bIsServer = Replication.IsServer();
if (!m_bIsServer)
    return;
if (!deployed)
    return;

BaseWorld baseWorld = owner.GetWorld();

vector mat[4];
owner.GetTransform(mat);

// Get physics properties
float mass = rb.GetMass();
float airspeed = rb.GetVelocity().Length();
vector forward = mat[2].Normalized();
vector right = mat[0].Normalized();
vector up = mat[1].Normalized();

// === THROTTLE (FORWARD PROPULSION) ===
vector thrustForce = forward * throttleInput * maxThrust * timeSlice;
rb.ApplyForce(thrustForce);

// === LIFT FORCE (SPEED² * LIFT COEFFICIENT) ===
float liftForce = airspeed * airspeed * liftCoefficient * wingArea;
vector lift = up * liftForce * timeSlice;
rb.ApplyForce(lift);

// === DRAG (AIR RESISTANCE) ===
vector drag = -rb.GetVelocity().Normalized() * (dragCoefficient * airspeed);
rb.ApplyForce(drag * timeSlice);

// === AUTO-STABILIZATION (KEEP PLANE LEVEL) ===
float pitchError = forward[1];  // Nose up/down
float rollError = right[1];      // Banking left/right

vector angularVelocity = rb.GetAngularVelocity();
vector stabilizationTorque =
    (-forward * rollError * balanceStrength) + 
    (right * pitchError * balanceStrength) - 
    (angularVelocity * dampingFactor);

rb.ApplyTorque(stabilizationTorque * timeSlice);

// === CONTROL SURFACES (PITCH, ROLL, YAW) ===
// Elevator (Pitch Control)
rb.ApplyTorque(right * moveInput[2] * pitchTorque * timeSlice);

// Ailerons (Roll Control)
rb.ApplyTorque(forward * moveInput[1] * rollTorque * timeSlice);

// Rudder (Yaw Control)
rb.ApplyTorque(up * yawInput * yawTorque * timeSlice);

// === SPEED LIMIT ===
rb.SetVelocity(rb.GetVelocity().Normalized() * Math.Min(speedCap, rb.GetVelocity().Length()));


		foreach (DroneWingSpine wing : wings)
		{
			//wing.SetSpinSpeed((throttleInput + vInput[2] + vInput[0] + yawInput) * throttleForce * mul * timeSlice);
		}

		//batteryHandler.EnergyUsed(Math.AbsFloat(throttleInput) + Math.AbsFloat(yawInput) + Math.AbsFloat(vInput[2]) + Math.AbsFloat(vInput[0]));
		if (batteryHandler.isDead())
			ClientDisconnect();

		if (droneSignalHandler.isOutOfRange())
			ClientDisconnect();
	}
	
}
