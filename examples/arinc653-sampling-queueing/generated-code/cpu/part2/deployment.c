#include <arinc653/types.h>
#include "deployment.h"


// variables used by the RECEIVE calls
extern MESSAGE_ADDR_TYPE SD_3_msg;
extern MESSAGE_ADDR_TYPE QD_5_msg;

// mapping between input ports and addresses of the variables used by the RECEIVE calls.
MESSAGE_ADDR_TYPE *receiving_addresses[POK_CONFIG_NB_SYSTEM_PORTS] =
{NULL,NULL,NULL,&SD_3_msg,NULL,&QD_5_msg,NULL,NULL};


// input buffers used by the message pre load phase (sampling ports)
unsigned char *input_sampling_buffer_1[POK_CONFIG_MAX_MESSAGE_SIZE];

unsigned char **input_buffers[POK_CONFIG_PART_NB_INPUT_SAMPLING_PORTS ]= {
input_sampling_buffer_1};

// input buffers used by the message pre load phase (queuing ports)
unsigned char input_queuing_buffer_1_msgs [POK_CONFIG_PART_MAX_QUEUING_MESSAGES][POK_CONFIG_MAX_MESSAGE_SIZE];
unsigned char *input_queuing_buffer_1[POK_CONFIG_PART_MAX_QUEUING_MESSAGES] = {
input_queuing_buffer_1_msgs[0],
input_queuing_buffer_1_msgs[1]};


unsigned char **input_buffers_queuing[POK_CONFIG_PART_NB_INPUT_QUEUING_PORTS] = {
input_queuing_buffer_1};
