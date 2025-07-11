#include <sensor_manager.h>

SensorManager::SensorManager(uint8_t pH_pin, uint8_t DO_pin, uint8_t roomTemperaturePin, uint8_t waterTemperaturePin, Preferences &prf)
    : ads(), ph(prf), oneWireRoom(roomTemperaturePin), roomTemperature(&oneWireRoom), oneWireWater(waterTemperaturePin), waterTemperature(&oneWireWater), preferences(prf)
{
    this->pH_pin = pH_pin;
    this->DO_pin = DO_pin;
}

void SensorManager::begin()
{
    EEPROM.begin(32); // Initialize EEPROM with 32 bytes

    Wire.begin();          // Initialize I2C communication
    ads.begin();           // Initialize the ADS1115
    ads.setGain(GAIN_ONE); // Set gain to 1

    ph.begin(); // Initialize the pH sensor

    roomTemperature.begin();  // Initialize the room temperature sensor
    waterTemperature.begin(); // Initialize the water temperature sensor

    this->doBegin(); // Initialize the DO sensor calibration values
}

// float SensorManager::readDO()
// {
//     uint16_t rawADC = ads.readADC_SingleEnded(this->DO_pin); // Read the ADC value from the DO pin
//     this->do_voltage = ads.computeVolts(rawADC) * 1000;      // Convert to millivolts

//     uint16_t V_saturation = (uint32_t)doCallibrationVoltage + (uint32_t)35 * (uint8_t)water_temperature - (uint32_t)doCallibrationTemperature * 35;
//     return (this->do_voltage * DO_Table[(uint8_t)water_temperature] / V_saturation) / 1000.0; // Calculate DO value (mg/L)
// }

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
    getTemperature();          // Get the temperature values
    this->DO_value = readDO(); // Read the DO value
    this->pH_value = readpH(); // Read the pH value
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