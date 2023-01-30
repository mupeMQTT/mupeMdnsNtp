
// Copyright Peter Müller mupe
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit
// persons to whom the Software is furnished to do so, subject to the
// following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
// NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
// USE OR OTHER DEALINGS IN THE SOFTWARE.

#include <stdio.h>
#include "mupeMdnsNtp.h"
#include "nvs_flash.h"
#include "mdns.h"
#include <esp_sntp.h>

esp_err_t start_mdns_service() {
	//initialize mDNS service
	size_t strSize=0;
	nvs_handle_t my_handle;
	char *url;
	nvs_open("WIFIcfg", NVS_READWRITE, &my_handle);
	nvs_get_str(my_handle, "url", NULL, &strSize);
	if (strSize == 0) {
		url = malloc(5);
		url[0] = 'm';
		url[1] = 'q';
		url[2] = 't';
		url[3] = 't';
		url[4] = 0;
		nvs_set_str(my_handle, "WIFIcfg", url);
		nvs_commit(my_handle);
		nvs_close(my_handle);
	} else {
		url = malloc(strSize);
		nvs_get_str(my_handle, "url", url, &strSize);
		nvs_close(my_handle);
	}
	esp_err_t err = mdns_init();
	if (err) {
		printf("MDNS Init failed: %d\n", err);
		return err;
	}
	//set hostname
	mdns_hostname_set(url);
	//set default instance
	mdns_instance_name_set("Peters test ESP");
	mdns_service_add(NULL, "_http", "_tcp", 80, NULL, 0);
	mdns_service_add(NULL, "_mqtt", "_mqtt", 1883, NULL, 0);
	return err;
}

void mupeMdnsNtpInit(void) {

	ESP_ERROR_CHECK(start_mdns_service());
	sntp_setoperatingmode(SNTP_OPMODE_POLL);
	sntp_setservername(0, "pool.ntp.org");
	sntp_init();
}

uint64_t getNowMs() {
	uint64_t t;
	struct timeval tv;
	gettimeofday(&tv, NULL);
	t = tv.tv_sec * 1000LL + (tv.tv_usec / 1000LL);
	return t;
}

void waitForNTPConnect() {
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = pdMS_TO_TICKS(1000);
	xLastWakeTime = xTaskGetTickCount();
	if (getNowMs() < 1500000000000) {
		vTaskDelayUntil(&xLastWakeTime, xFrequency);
	}

}
