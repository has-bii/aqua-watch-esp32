#include <sensor_manager.h>

// Define the static member variable
volatile int SensorManager::flowPulseCount = 0;

SensorManager::SensorManager(uint8_t pH_pin, uint8_t DO_pin, uint8_t roomTemperaturePin, uint8_t waterTemperaturePin, uint8_t flowSensorPin, uint8_t turbidityPin, Preferences &prf)
    : ads(), ph(prf), oneWireRoom(roomTemperaturePin), roomTemperature(&oneWireRoom), oneWireWater(waterTemperaturePin), waterTemperature(&oneWireWater), preferences(prf)
{
    this->pH_pin = pH_pin;
    this->DO_pin = DO_pin;
    this->FLOW_SENSOR_PIN = flowSensorPin;
    this->TURBIDITY_PIN = turbidityPin;
}

void SensorManager::begin()
{
    EEPROM.begin(32); // Initialize EEPROM with 32 bytes

    Wire.begin(); // Initialize I2C communication
    ads.begin();  // Initialize the ADS1115
    ads.setGain(GAIN_TWOTHIRDS);

    ph.begin(); // Initialize the pH sensor

    roomTemperature.begin();  // Initialize the room temperature sensor
    waterTemperature.begin(); // Initialize the water temperature sensor

    this->doBegin(); // Initialize the DO sensor calibration values

    this->flowRateBegin(); // Initialize the flow rate sensor
}

float SensorManager::readDO()
{
    // Temperature bounds checking
    if (water_temperature < 0.0f || water_temperature > 40.0f)
    {
        return -1.0f; // Error: temperature out of DO table range
    }

    uint16_t rawADC = ads.readADC_SingleEnded(this->DO_pin);
    this->do_voltage = ads.computeVolts(rawADC) * 1000.0f; // Convert to millivolts

    // Use float arithmetic to avoid overflow
    float V_saturation = (float)doCallibrationVoltage +
                         35.0f * water_temperature -
                         (float)doCallibrationTemperature * 35.0f;

    // Check for invalid saturation voltage
    if (V_saturation <= 0.0f)
    {
        return -1.0f; // Error: invalid saturation voltage
    }

    // Safe array access with bounds checking
    uint8_t temp_index = (uint8_t)constrain(water_temperature, 0, 40);

    // Calculate DO in mg/L (DO_Table values are already scaled)
    return (this->do_voltage * DO_Table[temp_index] / V_saturation) / 1000.0f;
}

float SensorManager::readpH()
{
    // ph_voltage = ads.readADC_SingleEnded(this->pH_pin) / 10;
    this->ph_voltage = ads.computeVolts(ads.readADC_SingleEnded(this->pH_pin)) * 1000; // Read the ADC value from the pH pin and convert to millivolts
    this->pH_value = ph.getPHValue(ph_voltage);                                        // Calculate pH value
    return this->pH_value;
}

void SensorManager::readSensors()
{
    getTemperature();                // Get the temperature
    this->readTurbidity();           // Read the turbidity value
    this->DO_value = readDO();       // Read the DO value
    this->pH_value = readpH();       // Read the pH
    this->flowRate = readFlowRate(); // Read the flow rate
}

float SensorManager::getDOValue()
{
    return this->DO_value; // Return the DO value
}

float SensorManager::getpHValue()
{
    return this->pH_value; // Return the pH value
}

bool SensorManager::phCallibration(uint8_t mode)
{
    switch (mode)
    {
    case 0:
        this->ph.setNeutralVoltage(0);
        this->ph.setAcidVoltage(0);
        return true;
        break;

    case 4:
        this->ph.setAcidVoltage(this->ph_voltage);
        return true;
        break;

    case 7:
        this->ph.setNeutralVoltage(this->ph_voltage);
        return true;
        break;

    default:
        return false;
        break;
    }
}

void SensorManager::getTemperature()
{
    roomTemperature.requestTemperatures();
    waterTemperature.requestTemperatures();

    this->room_temperature = roomTemperature.getTempCByIndex(0);   // Get the room temperature
    this->water_temperature = waterTemperature.getTempCByIndex(0); // Get the water temperature
}

void SensorManager::doBegin()
{
    this->preferences.begin("do_sensor", false);
    this->doCallibrationVoltage = this->preferences.getUInt("do_callibration_voltage", 0);
    this->doCallibrationTemperature = this->preferences.getUInt("do_callibration_temperature", 0);
    this->preferences.end();
}

void SensorManager::turbidityBegin()
{
    this->preferences.begin("turbidity_sensor", false);
    this->turbidityZeroOffset = this->preferences.getUInt("turbidity_zero_offset", 2500);      // Default zero offset for turbidity
    this->turbidtyHundredOffset = this->preferences.getUInt("turbidity_hundred_offset", 4100); // Default hundred offset for turbidity
    this->preferences.end();
}

bool SensorManager::doCalibration(float current_temperature)
{
    // Validate temperature range
    if (current_temperature < 0.0f || current_temperature > 40.0f)
    {
        return false; // Invalid temperature for calibration
    }

    // Read current voltage
    uint16_t rawADC = ads.readADC_SingleEnded(this->DO_pin);
    uint16_t current_voltage = ads.computeVolts(rawADC) * 1000.0f;

    this->doCallibrationVoltage = current_voltage;
    this->doCallibrationTemperature = (uint16_t)current_temperature;

    // Save to preferences
    this->preferences.begin("do_sensor", false);
    this->preferences.putUInt("do_callibration_voltage", current_voltage);
    this->preferences.putUInt("do_callibration_temperature", (uint16_t)current_temperature);
    this->preferences.end();

    return true;
}

bool SensorManager::turbidityCallibration(uint8_t mode)
{
    switch (mode)
    {
    case 0:                                 // Reset calibration
        this->turbidityZeroOffset = 2500;   // Default zero offset for turbidity
        this->turbidtyHundredOffset = 4100; // Default hundred offset
        this->preferences.begin("turbidity_sensor", false);
        this->preferences.putUInt("turbidity_zero_offset", this->turbidityZeroOffset);
        this->preferences.putUInt("turbidity_hundred_offset", this->turbidtyHundredOffset);
        this->preferences.end();
        return true; // Reset successful
        break;
    case 1: // Set zero offset
    {
        uint16_t rawADC = ads.readADC_SingleEnded(this->TURBIDITY_PIN);
        this->turbidityZeroOffset = ads.computeVolts(rawADC) * 1000; // Read the current voltage and set as zero offset
        this->preferences.begin("turbidity_sensor", false);
        this->preferences.putUInt("turbidity_zero_offset", this->turbidityZeroOffset);
        this->preferences.end();
        return true;
    }
    case 2: // Set hundred offset
    {
        uint16_t rawADC = ads.readADC_SingleEnded(this->TURBIDITY_PIN);
        this->turbidtyHundredOffset = ads.computeVolts(rawADC) * 1000; // Read the current voltage and set as hundred offset
        this->preferences.begin("turbidity_sensor", false);
        this->preferences.putUInt("turbidity_hundred_offset", this->turbidtyHundredOffset);
        this->preferences.end();
        return true;
    }
    default:
        return false; // Invalid mode
    }
}

void SensorManager::flowRateBegin()
{
    pinMode(FLOW_SENSOR_PIN, INPUT);
    flowPulseCount = 0;
    attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), pulseCounter, RISING);
}

void SensorManager::pulseCounter()
{
    flowPulseCount++; // Increment the pulse count
}

float SensorManager::readFlowRate()
{
    noInterrupts();
    int flowRate = flowPulseCount;
    flowPulseCount = 0; // Reset the pulse count
    interrupts();

    float flowRateLpm = (flowRate / 7.5f); // Convert pulse count to liters per minute

    return flowRateLpm; // Return the flow rate in L/min
}

float SensorManager::readTurbidity()
{
    uint16_t rawADC = ads.readADC_SingleEnded(this->TURBIDITY_PIN); // Read the turbidity sensor value
    this->turbidity_voltage = ads.computeVolts(rawADC) * 1000;      // Convert to millivolts

    // Calculate turbidity using the zero and hundred offsets
    this->turbidity = ((this->turbidity_voltage - this->turbidityZeroOffset) * 100.0f) / (this->turbidtyHundredOffset - this->turbidityZeroOffset);
    this->turbidity = constrain(this->turbidity, 0.0f, 100.0f); // Ensure turbidity is within 0-100 NTU

    return this->turbidity; // Return the turbidity value
}