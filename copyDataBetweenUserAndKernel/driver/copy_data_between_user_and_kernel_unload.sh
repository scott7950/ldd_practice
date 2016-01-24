#!/bin/bash
module="copy_data_between_user_and_kernel"
device="copy_data"

rmmod ${module}
rm -f /dev/${device}0
