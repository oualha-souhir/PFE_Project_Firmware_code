
#include <charging.h>
#include <math.h>
 extern double balance;
 extern float totalenergy;
extern bool send;
extern int station_id;
extern int initialized;
int i;
int charge;
extern char msg[256];
extern TaskHandle_t xTaskToDelete;
extern float Voltage, Current;
static float energy;
float totalenergy = 0;
bool send = true;
 float n;
extern bool web;
extern uint64_t rfid;

void set_cp_pwm_duty(uint32_t duty) {
    ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, duty);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
}

/**
 * @brief Function to start the charging process.
 * 
 * This function is responsible for managing the charging process. It checks the 
 * charging conditions, calculates energy consumption, updates the balance, and 
 * sends relevant information to the server.
 * 
 * @return char* Returns a string containing the charging status and other relevant information.
 */
char *start_charging()
    {

        energy = 0;
        Current = 0;
        Voltage = 0;
        
        // time_t start_time;

        //n =  read_cp(CP_READ);
        
        printf("CP value : %.2fV\n", n);
        if (n >= 1.242 && n <= 1.863)
        {   
            // int pp_value = adc1_get_raw(PP_PIN);
            // printf("PP value : %.2dV\n", pp_value);

        // xTaskCreate(receive, "listen_task", 4096, NULL, 6, &xTaskToDelete);

        if(i==0){
               // xTaskCreatePinnedToCore(guiTask, "gui", 4096 * 2, NULL, 0, NULL, 1);

            if (!web){
                sprintf(msg, "{\"status\":1,\"rfid\":%llu, \"station_id\":%d,\"web\":0 }", rfid, station_id);
            }else if (web){
                sprintf(msg, "{\"status\":1,\"rfid\":%llu, \"station_id\":%d ,\"web\":1}", rfid, station_id);
            }
        
        // time(&start_time);
        }

        i++;
        gpio_set_level(DC_RELAY_CONTACTOR, 0);
        charge = 1;
        printf("i : %d\n", i);
        
        //* simulation of unplugging the car
        
        // if (i==3){
        //             n = 2;
        //         }
        // printf("i : %d, n: %f\n", i,n);
        //*

        // time_t now;
        // time(&now);
        // printf("    start charging time %lld\n", start_time);
        // printf("    Current time : %lld\n", now);
        

        // time_t time_passed = now - start_time ;
        // printf("    time_passed : %lld\n", time_passed);
        int time_passed = i ;

        float Voltage = read_volt(VOLTAGE_MODULE); // Replace with the correct channel for Voltage
        
        //printf("    Voltage : %.2fV\n", Voltage);
        Voltage = 220;
        float Current = read_Current(CURRENT_MODULE); // Replace with the correct channel for Current
        // printf("    Current : %.2fA\n", Current);  // Channel 3
        Current = 20;

        float pp_value = read_pp(PP);
        float min_value = fmin(Current, pp_value);
        cp_pwm( min_value);
        energy = energy_consumed(Voltage, Current, time_passed);
        totalenergy += energy;
        printf("totalenergy: %f\n", totalenergy);
        float charging_rate = 90;
        balance = balance - energy * charging_rate;
        if (balance < 0)
        {
            balance = 0;
        
        }

        Voltage++;
        Current++;
        printf("Remaining balance: %.2f\n", balance);
        // vTaskDelete(xTaskToDelete);
        if (web && i!=1){
        sprintf(msg, "{\"Voltage\":%f,\"Current\":%f,\"energy\":%f, \"balance\":%f}", Voltage, Current, energy, balance);
            //iothub_send(msg);
            
            
        }else if (!web && i!=1){
            send = false;
        }
        }
    else if (n >= 1.863 )
    {
        printf("Car not plugged\n");
        gpio_set_level(DC_RELAY_CONTACTOR, 1);
        charge = 0;
        initialized = 0;
        printf("Charging session ended\n");
        if (!web){
            sprintf(msg, "{\"rfid\":%llu,\"balance\":%f}", rfid, balance);
            //iothub_send(msg);
            
            // vTaskDelete(xTaskToDelete);
        }
        // sprintf(msg, "{\"start\":0}");
        
        
        
    }else 
    {
        printf("error\n");
        gpio_set_level(DC_RELAY_CONTACTOR, 1);
        charge = 0;
        initialized = 0;
        printf("Charging session ended\n");
        if (!web){
            sprintf(msg, "{\"rfid\":%llu,\"balance\":%f}", rfid, balance);
            //iothub_send(msg);
            

            //  vTaskDelete(xTaskToDelete);
        }

        sprintf(msg, "{\"start\":0}");
        totalenergy = 0;
        
        // vTaskDelete(xTaskToDelete);
    }

    
    
    return msg;
}

/**
 * @brief The charging task function.
 * 
 * This function is responsible for managing the charging process. It runs as a task in the FreeRTOS environment.
 * The task continuously checks the balance and initialized variables to determine if charging should continue.
 * If charging is ongoing, it calls the start_charging function, sends the returned message to the server, and delays for stability.
 * After the charging session ends (when balance reaches 0 or initialized is set to 0), it sends a final message to the server,
 * releases the DC relay contactor, resets the charge and i variables, resets the totalenergy variable, and sends a confirmation message.
 * 
 * @param pvParameters A pointer to the task parameters. In this case, it is not used, so it is set to void*.
 */
void charging_task(void* pvParameters) {

    while (balance > 0 && initialized == 1)
    {
        strcpy(msg, start_charging(balance));
        if(send ){
            printf("msg: %s\n", msg);
            iothub_send(msg);
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS); // Delay for stability

    }
    gpio_set_level(DC_RELAY_CONTACTOR, 1);
    sprintf(msg, "{\"status\":0,\"energy\":%f,\"Balance\":%f,\"rfid\":%llu,\"station_id\":%d}",totalenergy, balance, rfid, station_id);
    printf("msg: %s\n", msg);
    charge = 0;
    i = 0;
    totalenergy = 0;
    iothub_send(msg);
    send = true;
    vTaskDelete(NULL);
}