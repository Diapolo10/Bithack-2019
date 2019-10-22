import socket
import time
import struct
import threading
import queue
from datetime import datetime
import random
import logging
BITBOX_UDP_IP = '127.0.0.1'
BITBOX_UDP_PORT = 8001

KEY_SPEED = "speed"
KEY_RPM = "rpm"
KEY_TEMPERATURE = "temperature"
KEY_FUEL = "fuel"

RANDOM = [KEY_SPEED, KEY_RPM, KEY_TEMPERATURE, KEY_FUEL]

MESSAGES = {
    KEY_SPEED: {"ID": 0x10, "value": 70, "min": 0, "max": 250, "increase": True, "index": 0 },
    KEY_RPM: {"ID": 0x20, "value": 0, "min": 0, "max": 8000, "increase": True, "index": 0 },
    KEY_TEMPERATURE: {"ID": 0x30, "value": 70, "min": 70, "max": 110, "increase": True, "index": 0 },
    KEY_FUEL: {"ID": 0x40, "value": 0, "min": 0, "max": 60, "increase": True, "index": 0 }
}

STEP_SMALL = 1
STEP_LARGE = 10
PERIOD_MS = 10
START_TIME = 0

SIZE_MSG_START = 8
SIZE_MSG_ID = 8
SIZE_MSG_TIMESTAMP = 32
SIZE_MSG_VALUE = 16
SIZE_MSG_END = 8
# Total: 72 bits

MESSAGE_START = "{:02d}".format(0)
MESSAGE_END = hex(255)[2:]
FAULT_START = hex(15)[2:].zfill(2)


def millis_from_start(start):
    return int(round((time.time() - start) * 1000))


def current_milli_time():
    return int(round(time.time() * 1000))


def getFaultMessage(msg):
    # Do not touch, please
    now = datetime.now()
    timestamp = hex(int(datetime.timestamp(now)))[2:]
    msgId = hex(int(MESSAGES[msg]["ID"]))[2:]
    value = '0x{0:0{1}X}'.format(MESSAGES[msg]["ID"], 4)[2:]
    message = str(
        "".join([MESSAGE_START, FAULT_START, timestamp, value, MESSAGE_END]))
    bytearr = bytearray.fromhex(message)
    return bytearr


def getMessageBytes(msg):
    # Do not touch, please
    # Every byte is formatted in a way that 8 bits is presented with two chars.
    now = datetime.now()
    timestamp = hex(int(datetime.timestamp(now)))[2:]
    msgId = hex(int(MESSAGES[msg]["ID"]))[2:]
    value = '0x{0:0{1}X}'.format(MESSAGES[msg]["value"], 4)[2:]
    message = str(
        "".join([MESSAGE_START, msgId, timestamp, value, MESSAGE_END]))
    bytearr = bytearray.fromhex(message)
    return bytearr


def calcChangedValues():
    calcValue(KEY_SPEED, STEP_LARGE)
    calcValue(KEY_RPM, STEP_LARGE)
    calcValue(KEY_TEMPERATURE, STEP_SMALL)
    calcValue(KEY_FUEL, STEP_SMALL)


def calcValue(msg, step):
    obj = MESSAGES[msg]


    if obj["index"] % 20 != 0:
        step = 0

    obj["index"] += 1

    if obj["increase"] is True:
        if obj["value"] + step <= obj["max"]:
            obj["value"] += step
        else:
            obj["increase"] = False
    else:
        if obj["value"] - step >= obj["min"]:
            obj["value"] -= step
        else:
            obj["increase"] = True

def main():
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    global START_TIME, KEY_SPEED, KEY_RPM, KEY_TEMPERATURE, KEY_FUEL
    START_TIME = time.time()

    counter = 0
    RandomFaultcount = 1
    rmpFaultCount = 1

    rpmPatternIndex = 0
    rpmSend = True
    rpmSendPattern = [3, 3.6, 41.1, 41.7]

    while True:
        counter += 1
        elapsed_time = (time.time() - START_TIME)
        start = current_milli_time()

        logging.warning(elapsed_time)
        if rpmPatternIndex < len(rpmSendPattern) and elapsed_time >= rpmSendPattern[rpmPatternIndex]:
            rpmSend = not rpmSend
            rpmPatternIndex += 1
        if rpmSend:
            s.sendto(getMessageBytes(KEY_RPM),
                 (BITBOX_UDP_IP, BITBOX_UDP_PORT))


        # Send each messages periodically
        s.sendto(getMessageBytes(KEY_SPEED), 
                (BITBOX_UDP_IP, BITBOX_UDP_PORT))
        s.sendto(getMessageBytes(KEY_TEMPERATURE),
                 (BITBOX_UDP_IP, BITBOX_UDP_PORT))
        s.sendto(getMessageBytes(KEY_FUEL),
                 (BITBOX_UDP_IP, BITBOX_UDP_PORT))

        # Calculate new values for Gauges        
        calcChangedValues()


        sleepTime = (PERIOD_MS - (current_milli_time() - start)) / 1000
        if sleepTime > 0:
            time.sleep(sleepTime)


        # Send Random faultmessage 3x in a minute
        if  RandomFaultcount == 1 and (elapsed_time > random.randrange(20,21) and elapsed_time < random.randrange(22,23)) or \
            RandomFaultcount == 2 and (elapsed_time > random.randrange(30,34) and elapsed_time < random.randrange(35,39)) or \
            RandomFaultcount == 3 and (elapsed_time > random.randrange(40,46) and elapsed_time < random.randrange(47,51)):

            RandomFaultcount += 1
            s.sendto(getFaultMessage(random.choice(RANDOM)),
                 (BITBOX_UDP_IP, BITBOX_UDP_PORT))

        # Reset
        if elapsed_time != 0 and elapsed_time >= 60 and RandomFaultcount != 3:
            RandomFaultcount = 1
            START_TIME = time.time()
            elapsed_time = 0
            counter = 0
        
        

    s.close()


if __name__ == "__main__":
    main()