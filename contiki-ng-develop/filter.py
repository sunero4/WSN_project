import re
from enum import Enum


class DataTypes(Enum):
    ENERGEST = 1
    PACKAGELOSS = 2
    RSSI = 3


sourceFile = 'tsch_uden_jammer.txt'
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


packageLossFile = 'results/csma_uden_jammer_PackageLossData.txt'


def countPackageLoss():
    sending_counter = 0
    received_counter = 0
    with open(packageLossFile) as source:
        for line in source:
            if 'Sending' in line:
                sending_counter = sending_counter + 1
            if 'Received' in line:
                received_counter = received_counter + 1
    print("Sending count: " + str(sending_counter))
    print("Received count: " + str(received_counter))


countPackageLoss()
