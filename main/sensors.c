#include "sensors.h"


float Voltage, Current;
int adc_max = 3024;       // Variable to store the maximum sensor value
int adc_min = 2256;    // Variable to store the minimum sensor value

    float volt_multi_p;               // Peak voltage
    float volt_multi_n;               // Negative peak voltage
   float volt_multi = 220;  
  
long map(long x, long in_min, long in_max, long out_min, long out_max) {
    const long run = in_max - in_min;
    if(run == 0){
        printf("map(): Invalid input range, min == max");
        return -1; // AVR returns -1, SAM returns 0
    }
    const long rise = out_max - out_min;
    const long delta = x - in_min;
    return (delta * rise) / run + out_min;
}

// Function to get the time in milliseconds since the program started
uint64_t millis() {
    return esp_timer_get_time() / 1000;
}


float read_cp(adc1_channel_t channel) {
    float R1 = 47;
    float R2 = 180;
    int readValue = adc1_get_raw(channel);
    printf("readValue:%d\n",readValue);
    float adc_voltage = readValue * 3.3 / 4096;
    printf("adc_voltage:%f\n",adc_voltage);
     float CP_voltage=((adc_voltage * (R1+R2))/R2);
    printf("CP_voltage: %f\n", CP_voltage);

    return CP_voltage;
}

float read_pp(adc1_channel_t channel) {
    float R1 = 1;
    float R2 = 1;
    int readValue = adc1_get_raw(channel);
    printf("readValue:%d\n",readValue);
    float adc_voltage = readValue * 3.3 / 4096;
    printf("adc_voltage:%f\n",adc_voltage);
     float CP_voltage=((adc_voltage * (R1+R2))/R2);
    printf("CP_voltage: %f\n", CP_voltage);

    return CP_voltage;
}

void cp_pwm(float current) {
        int duty_cycle = current / 0.6;  // Calculate duty cycle in percentage
    // 1024 = 10-bit resolution
    // 1024 * (duty_cycle / 100) gives the duty cycle in bits
    uint32_t duty_cycle_res = 1024 * (duty_cycle / 100.0); 
    set_cp_pwm_duty(duty_cycle_res);
}
float read_volt(adc1_channel_t channel) {
    float adc_sample;
    float volt_inst = 0;
    float sum = 0;
    float volt;
    long init_time = millis();
    int N = 0;
    
    while ((millis() - init_time) < 500) {   // Duration of 0.5 seconds (Approximately 30 cycles of 60Hz)
        adc_sample = adc1_get_raw(channel);           // Sensor voltage
        volt_inst = map(adc_sample, adc_min, adc_max, volt_multi_n,volt_multi_p );
        sum = sum+(volt_inst*volt_inst );
        N++;
        vTaskDelay(1 / portTICK_PERIOD_MS); // Short delay to avoid busy-waiting
    }

    volt = sqrt(sum / N);                     // RMS equation
    return volt;
}


float read_Current(adc1_channel_t channel) {   
    float R1 = 680;
float R2 = 330;
float R3 = 22;
      int readValue=0;                // value read from the sensor
  int maxValue = 0;             // store max value here
  int minValue = 4096;          // store min value here ESP32 ADC resolution
  
    float sensorOffset = 2.15;
    uint32_t start_time = millis();
   while((millis()-start_time) < 1000) //sample for 1 Sec
   {
       readValue = adc1_get_raw(channel);
       // see if you have a new maxValue
       if (readValue > maxValue) 
       {
           /*record the maximum sensor value*/
           maxValue = readValue;
       }
       if (readValue < minValue) 
       {
           /*record the minimum sensor value*/
           minValue = readValue;
       }
   }
   
    

    float adc_voltage = (maxValue - minValue) * 3.3 / 4096.0;
    //  float current_voltage=(adc_voltage * (R1+R2+R3)/R1);

    float current_voltage = adc_voltage ;
    float VRMS = (current_voltage/2.0) *0.707; 
    float current = ((VRMS * 1000)/66)-0.2; 
    if (current <= 0.0) 
        current = 0;
    return  current ;
}


float energy_consumed(float Voltage, float Current, time_t time_seconds) {

    printf("Current : %.2fA\n", Current);
    printf("Voltage : %.2fV\n", Voltage);
    printf("time_seconds: %lld\n", time_seconds);
    float energy_kwh = (Voltage * Current * time_seconds) / (1000.0 * 3600.0);
    printf("Energy Consumed: %f kWh\n", energy_kwh);
    return energy_kwh;
}

void configure_adc() {
    //gpio_pad_select_gpio(DC_RELAY_CONTACTOR);  
    esp_rom_gpio_pad_select_gpio(DC_RELAY_CONTACTOR);  
    gpio_set_direction(DC_RELAY_CONTACTOR, GPIO_MODE_OUTPUT);
    gpio_set_level(DC_RELAY_CONTACTOR, 1);
    adc1_config_width(ADC_WIDTH_BIT_12); 
    adc1_config_channel_atten(CURRENT_MODULE, ADC_ATTEN_DB_12);
    adc1_config_channel_atten(VOLTAGE_MODULE, ADC_ATTEN_DB_12);
    adc1_config_channel_atten(CP_READ, ADC_ATTEN_DB_12);

 volt_multi_p = volt_multi * 1.4142;   // Peak voltage = RMS voltage * 1.4142 (Single-phase current)
  volt_multi_n = -volt_multi_p;         // Negative peak voltage
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_HIGH_SPEED_MODE,
        .timer_num        = LEDC_TIMER_0,
        .duty_resolution  = LEDC_TIMER_12_BIT,
        .freq_hz          = 500,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ledc_timer_config(&ledc_timer); 

    ledc_channel_config_t ledc_channel = {
        .speed_mode     = LEDC_HIGH_SPEED_MODE,
        .channel        = LEDC_CHANNEL_0,
        .timer_sel      = LEDC_TIMER_0,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = CP_PWM_PIN,
        .duty           = 0,
        .hpoint         = 0
    };
    ledc_channel_config(&ledc_channel);
}
