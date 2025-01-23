class NetworkTimer {
        float timer;
        float MinTimeBetweenTicks;
        int CurrentTick ;
        
        void NetworkTimer(float serverTickRate) {
            MinTimeBetweenTicks = 1 / serverTickRate;
        }

        void Update(float deltaTime) {
            timer += deltaTime;
        }
        
        bool ShouldTick() {
            if (timer >= MinTimeBetweenTicks) {
                timer -= MinTimeBetweenTicks;
                CurrentTick++;
                return true;
            }

            return false;
        }
}