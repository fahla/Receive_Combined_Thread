#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>
#include <zephyr/net/openthread.h>
#include <openthread/thread.h>
#include <openthread/udp.h>
#include <stdio.h>

#define SLEEP_TIME_MS 10000

void udpReceiveCb(void *aContext, otMessage *aMessage,
 const otMessageInfo *aMessageInfo){
uint32_t payloadLength = otMessageGetLength(aMessage) - otMessageGetOffset(aMessage);
char buf[payloadLength+1];
otMessageRead(aMessage, otMessageGetOffset(aMessage), buf, payloadLength);
buf[payloadLength]='\0';

uint32_t co2_value, tvoc_value;
    
    // Parse the received string to extract CO2 and TVOC values
    if (sscanf(buf, "eCO2: %u ppm, eTVOC: %u ppb", &co2_value, &tvoc_value) == 2) {
        // Print the extracted values
        printk("Received eCO2 value: %u ppm\n", co2_value);
        printk("Received eTVOC value: %u ppb\n", tvoc_value);
    } else {
        // Handle parsing error if sscanf does not extract both values
        printk("Error parsing received data\n");
    }
}

void udpReceiveCbSCD(void *aContext, otMessage *aMessage,
const otMessageInfo *aMessageInfo){
uint32_t payloadLength = otMessageGetLength(aMessage) - otMessageGetOffset(aMessage);
char buff[payloadLength+1];
otMessageRead(aMessage, otMessageGetOffset(aMessage), buff, payloadLength);
buff[payloadLength]='\0';

uint16_t co2;  
int32_t temperature, humidity;
    
    // Parse the received string to extract CO2 and TVOC values
    if (sscanf(buff, "CO2: %u ppm \n Temperature: %d °C\n Humidity: %d percent RH", &co2, &temperature, &humidity) == 3) {
        // Print the extracted values
        printk("Received CO2 value: %u ppm\n", co2);
        printk("Received temperature: %d °C\n", temperature);
		printk("Received humidity: %d percent RH\n", humidity);
    } else {
        // Handle parsing error if sscanf does not extract both values
        printk("Error parsing received data\n");
    }
}

void udpReceiveCbSPS(void *aContext, otMessage *aMessage,
const otMessageInfo *aMessageInfo){
uint32_t payloadLength = otMessageGetLength(aMessage) - otMessageGetOffset(aMessage);
char buffs[payloadLength+1];
otMessageRead(aMessage, otMessageGetOffset(aMessage), buffs, payloadLength);
buffs[payloadLength]='\0';

  float pm1_0, pm2_5, pm4_0, pm10_0, nc0_5, nc1_0, nc2_5, nc10_0, typical_size;
    int ret = sscanf(buffs, 
                     "pm1.0: %f, pm2.5: %f, pm4.0: %f, pm10.0: %f, nc0.5: %f, nc1.0: %f, nc2.5: %f, nc10.0: %f, typical size: %f", 
                    &pm1_0, &pm2_5, &pm4_0, &pm10_0, &nc0_5, &nc1_0, &nc2_5, &nc10_0, &typical_size);
	if (ret != 0) {
            //printk("Parsed values:\n");
        printk("pm1.0: %.3f\n", pm1_0);
        printk("pm2.5: %.3f\n", pm2_5);
        printk("pm4.0: %.3f\n", pm4_0);
        printk("pm10.0: %.3f\n", pm10_0);
        printk("nc0.5: %.3f\n", nc0_5);
        printk("nc1.0: %.3f\n", nc1_0);
        printk("nc2.5: %.3f\n", nc2_5);
        printk("nc10.0: %.3f\n", nc10_0);
        printk("typical size: %.3f\n", typical_size);
        //printk("\n");
      }
}

static void udp_init(void) {
	otError error = OT_ERROR_NONE;
	otInstance* myInstance = openthread_get_default_instance();
	otUdpSocket mySocket;
	otSockAddr mySockAddr;
	memset(&mySockAddr, 0, sizeof(mySockAddr));
	mySockAddr.mPort = 1234;
	otInstance* myInstance2 = openthread_get_default_instance();
	otUdpSocket mySocket2;
	otSockAddr mySockAddr2;
	memset(&mySockAddr2, 0, sizeof(mySockAddr2));
	mySockAddr2.mPort = 5678;
	otInstance* myInstance3 = openthread_get_default_instance();
	otUdpSocket mySocket3;
	otSockAddr mySockAddr3;
	memset(&mySockAddr3, 0, sizeof(mySockAddr3));
	mySockAddr3.mPort = 9876;
	do {
		error = otUdpOpen(myInstance, &mySocket, udpReceiveCb, NULL);
		if (error != OT_ERROR_NONE) { break; }
		error = otUdpBind(myInstance, &mySocket, &mySockAddr, OT_NETIF_THREAD);
	} while (false);
	do {
		error = otUdpOpen(myInstance2, &mySocket2, udpReceiveCbSCD, NULL);
		if (error != OT_ERROR_NONE) { break; }
		error = otUdpBind(myInstance2, &mySocket2, &mySockAddr2, OT_NETIF_THREAD);
	} while (false);
	do {
		error = otUdpOpen(myInstance3, &mySocket3, udpReceiveCbSPS, NULL);
		if (error != OT_ERROR_NONE) { break; }
		error = otUdpBind(myInstance3, &mySocket3, &mySockAddr3, OT_NETIF_THREAD);
	} while (false);
	if (error != OT_ERROR_NONE) {
		printk("init_udp error: %d\n", error);
	}
}

int main(void) {
	udp_init();
	while (1) {
		k_msleep(SLEEP_TIME_MS);
	}
}