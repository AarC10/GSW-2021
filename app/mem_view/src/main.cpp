#include <stdio.h>
#include <string.h>
#include <iostream>
#include <string>
#include <vector>
#include <stdint.h>
#include <unistd.h>
#include "lib/vcm/vcm.h"
// #include "lib/shm/shm.h"
#include "lib/telemetry/TelemetryViewer.h"
#include "lib/dls/dls.h"
#include "common/types.h"

// view telemetry memory live
// run as mem_view [-f path_to_config_file]

using namespace vcm;
using namespace shm;
using namespace dls;

int main(int argc, char* argv[]) {
    MsgLogger logger("mem_view");

    logger.log_message("starting mem_view");

    std::string config_file = "";

    for(int i = 1; i < argc; i++) {
        if(!strcmp(argv[i], "-f")) {
            if(i + 1 > argc) {
                logger.log_message("Must specify a path to the config file after using the -f option");
                printf("Must specify a path to the config file after using the -f option\n");
                exit(-1);
            } else {
                config_file = argv[++i];
            }
        } else {
            std::string msg = "Invalid argument: ";
            msg += argv[i];
            logger.log_message(msg.c_str());
            printf("Invalid argument: %s\n", argv[i]);
            exit(-1);
        }
    }

    VCM* vcm;
    if(config_file == "") {
        vcm = new VCM(); // use default config file
    } else {
        vcm = new VCM(config_file); // use specified config file
    }

    if(FAILURE == vcm->init()) {
        logger.log_message("failed to initialize VCM");
        printf("failed to initialize VCM\n");
        exit(-1);
    }

    TelemetryViewer tlm;
    if(FAILURE == tlm.init(vcm)) {
        logger.log_message("failed to initialize telemetry viewer");
        printf("failed to initialize telemetry viewer\n");
        exit(-1);
    }

    tlm.add_all();
    tlm.set_update_mode(TelemetryViewer::BLOCKING_UPDATE);

    unsigned int max_length = 0;
    size_t max_size = 0;
    for(std::string it : vcm->measurements) {
        if(it.length() > max_length) {
            max_length = it.length();
        }

        measurement_info_t* info = vcm->get_info(it);
        // assuming info isn't NULL since it's in the vcm list
        if(info->size > max_size) {
            max_size = info->size;
        }
    }

    // clear the screen
    printf("\033[2J");

    measurement_info_t* m_info;
    unsigned char* buff = new unsigned char[max_size];
    while(1) {
        for(std::string meas : vcm->measurements) {
            m_info = vcm->get_info(meas);

            printf("%s  ", meas.c_str());

            // print extra spaces
            for(size_t i = 0; i < max_length - meas.length(); i++) {
                printf(" ");
            }

            if(FAILURE == tlm.get_raw(m_info, (uint8_t*)buff)) {
                logger.log_message("failed to get raw telemetry value");
                printf("failed to get raw telemetry value\n");
                continue; // keep on going
            }

            for(size_t i = 0; i < m_info->size; i++) {
                printf("0x%02X ", buff[i]);
            }

            printf("\n");
        }

        // update telemetry
        if(FAILURE == tlm.update()) {
            logger.log_message("failed to update telemetry");
            printf("failed to update telemetry\n");
            // ignore and continue
        } else {
            // clear the screen
            printf("\033[2J");
        }

        usleep(1000);
    }
}
