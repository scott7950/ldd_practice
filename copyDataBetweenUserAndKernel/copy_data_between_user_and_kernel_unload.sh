#!/bin/bash
module="driver"
device="driver"

rmmod ${module}
rm -f /dev/${device}0
