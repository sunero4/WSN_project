import re
from enum import Enum
from decimal import Decimal


class DataTypes(Enum):
    ENERGEST = 1
    PACKAGELOSS = 2
    RSSI = 3


sourceFile = 'tsch_uden_jammer_PackageLossData.txt'
targetFilePrefix = 'results/tsch_uden_jammer'


def writeToTarget(line, dataType):
    targetFile = ""
    if dataType == DataTypes.ENERGEST:
        targetFile = targetFilePrefix + '_EnergestData.txt'
    if dataType == DataTypes.PACKAGELOSS:
        targetFile = targetFilePrefix + '_PackageLossData.txt'
    if dataType == DataTypes.RSSI:
        targetFile = targetFilePrefix + '_RSSIData.txt'
    with open(targetFile, 'a') as target:
        target.write(line)


def latencies():
    times_dict = {}
    send_count = 0
    count = 0
    with open("results/csma_med_jammer_PackageLossData.txt") as source:
        for line in source:
            ev = 0
            send = line.find("Sending")
            rec = line.find("Received")

            times = line.split(':', maxsplit=1)
            m = float(times[0]) * 60000.0
            sec = float(times[1][:6]) * 1000.0

            if send > -1:
                ev = line[send + 8:send + 16]
                times_dict[ev] = (m + sec, 0)
            else:
                ev = line[rec + 10:rec + 18]
                times_dict[ev] = (times_dict[ev][0], m + sec)

    send_sum = 0
    rec_sum = 0
    count = 0
    for key, value in times_dict.items():
        if(value[1] > 0):
            send_sum = send_sum + value[0]
            rec_sum = rec_sum + value[1]
            count = count + 1

    print((rec_sum - send_sum) / count)

    with open("resssss.txt", "w") as file:
        for key in times_dict.keys():
            l = key + ": " + \
                str(times_dict[key][1] - times_dict[key][0]) + "\n"
            file.write(l)


def extractData():
    regex = re.compile('.+ch.+q.+busy.+')
    with open(sourceFile) as source:
        for line in source:
            if 'ID:2' in line:
                if 'Energest' in line or 'CPU' in line or 'Radio LISTEN' in line:
                    writeToTarget(line, DataTypes.ENERGEST)
            if 'Event' in line or 'Alive' in line:
                writeToTarget(line, DataTypes.PACKAGELOSS)
            if 'TSCH Stats' in line or re.match(regex, line) or 'cs:' in line:
                writeToTarget(line, DataTypes.RSSI)


# extractData()
latencies()
