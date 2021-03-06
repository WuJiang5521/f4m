import getopt
import os
import random
import sys
import time
from datetime import datetime
from random import randrange

import math


class Event:
    def __key(self):
        return self.aid, self.sym

    def __init__(self, aid, sym):
        self.aid = aid
        self.sym = sym

    def toString(self):
        return str(self.aid) + '.' + str(self.sym)

    def __eq__(self, other):
        return self.aid == other.aid and self.sym == other.sym

    def __ne__(self, other):
        return not self == other

    def __hash__(self):
        return hash(self.__key())


class EventSet:
    def __init__(self):
        self.events = []

    def add(self, event):
        self.events.append(event)

    def toString(self):
        result = ''
        for i in range(0, len(self.events)):
            result = result + self.events[i].toString()
            if i != len(self.events) - 1:
                result = result + ', '
        return '{' + result + '}'


class Pattern:
    def __init__(self, support, gapChance, size, length):
        self.support = support
        self.gapChance = gapChance
        self.size = size
        self.length = length
        self.eventSets = []
        for i in range(0, length):
            self.eventSets.append(EventSet())

    def add(self, ts, aid, sym, uniqueset):
        event = Event(aid, sym)
        if uniqueset is not None:
            if event in uniqueset:
                return False
            else:
                uniqueset.add(event)
        self.eventSets[ts].add(event)
        return True

    def getAttributes(self):
        attr = set([])
        for evs in self.eventSets:
            for ev in evs.events:
                attr.add(ev.aid)
        return attr

    def toString(self):
        result = 'sup: ' + str(self.support) + ' gapChance: ' + str(self.gapChance) + ' sz: ' + str(
            self.size) + ' len: ' + str(self.length) + ' events: '
        for evs in self.eventSets:
            result = result + evs.toString()
        return result


def generateRandomPatterns(input_type, nrOfPatterns, minSup, maxSup, minGapChance, maxGapChance, minSize, maxSize,
                           nrAttr, alphaPerAttr, nrMev, unique):
    # a list of patterns that is returned
    patterns = []
    if unique:
        uniqueSet = set([])  # set to hold all used symbols
    else:
        uniqueSet = None

    for i in range(0, nrOfPatterns):
        gapChance = randrange(int(100 * minGapChance), int(100 * maxGapChance) + 1, 1) / 100.0
        size = randrange(minSize, maxSize + 1, 1)
        length = randrange(math.ceil(float(size) / nrAttr), size + 1, 1)
        support = randrange(int(minSup * 10), int(maxSup * 10) + 1, 1)  # as percentage of total #events
        if input_type == 1:
            support = int(nrMev * nrAttr * (support / 10000.0))
        else:
            support = int(nrMev * nrAttr * 0.5 * (
                    support / 10000.0))  # how likely is each attribute to appear in each timestep -> for now 0.5
        support = int(support / size)
        patterns.append(Pattern(support, gapChance, size, length))

        if unique and len(uniqueSet) + size > nrAttr * alphaPerAttr:
            return None

        sizePerTs = [1] * length  # every timestep gets at least one event
        # divide the rest of the events randomly
        rest = size - length
        while rest > 0:
            pos = randrange(0, length, 1)
            if sizePerTs[pos] < nrAttr:
                sizePerTs[pos] = sizePerTs[pos] + 1
                rest = rest - 1

        for l in range(0, length):
            # hold blacklist for used attributes -> every attribute can only have one value each timestep
            used = set([])
            for h in range(0, sizePerTs[l]):
                aid = randrange(0, nrAttr, 1)
                while aid in used:
                    aid = randrange(0, nrAttr, 1)
                sym = randrange(0, alphaPerAttr, 1)
                while True:
                    if patterns[i].add(l, aid, sym, uniqueSet):
                        break
                    aid = randrange(0, nrAttr, 1)
                    while aid in used:
                        aid = randrange(0, nrAttr, 1)
                    sym = randrange(0, alphaPerAttr, 1)
                used.add(aid)
    return patterns


def insertPattern(data, nrMev, pattern, overwrite, interleave, occupied):
    loc = ''
    cnt_not_inserted = 0
    no_room = False

    if not interleave:
        blacklist = [0 for _ in range(nrMev)]
        for i in range(nrMev):
            blacklist[i] = [False for _ in range(data[i])]

    for occ in range(0, pattern.support):
        start = int(time.time())
        success = False
        positions = [0] * pattern.length
        while not success:  # First: search for a location to insert
            eind = int(time.time())
            if eind - start > 2:
                print('ERROR: not able to insert the pattern: ' + pattern.toString())
                no_room = True
                break
            success = True
            pos = randrange(0, nrMev - pattern.length)
            for ts in range(0, pattern.length):
                pos = pos + 1
                while randrange(0, 101) / 100.0 < pattern.gapChance and (pos + (pattern.length - ts)) < nrMev:
                    pos = pos + 1
                positions[ts] = pos
                # insert all events at this timestep
                for event in pattern.eventSets[ts].events:
                    if not overwrite and occupied[positions[ts]][event.aid]:
                        success = False
                        break
                if not success:
                    break
            if not interleave:  # occurences of a single pattern may not interleave
                for i in range(positions[0], positions[-1] + 1):
                    for aid in pattern.getAttributes():  # for all attributes that this pattern overlaps
                        if occupied[i][aid]:
                            success = False
                            break
                    if not success:
                        break
        if no_room:
            cnt_not_inserted = cnt_not_inserted + (pattern.support - occ)
            print(f'#ACTUAL support: {pattern.support - cnt_not_inserted} in stead of {pattern.support}')
            break
        loc = loc + '['
        for ts in range(0, pattern.length):  # Second: insert the pattern
            pos = positions[ts]
            loc = loc + str(pos)
            if ts != pattern.length - 1:
                loc = loc + ', '
            for event in pattern.eventSets[ts].events:
                data[pos][event.aid] = event.sym
                occupied[pos][event.aid] = True
                if not interleave:
                    blacklist[pos][event.aid] = True
        loc = loc + '] '
    return int(pattern.support - cnt_not_inserted)


"""
CATEGORICAL
    Every line contains one attribute and ends with -2, all sequences are separated by -1
    For each attribute i its values range from 0 to max_i
ITEM SET
    For every time step all its events are listed subsequently and time steps are separated with -2, all sequences are separated by -1
    Each value ranges from 0 to max, where the value also indicates the id of the attribute 
"""


def generateData(input_type, nrMev, nrAttr, alphaPerAttr, patterns, overwrite, interleave):
    minsup = sys.maxsize

    g_mev_time = [0] * nrMev  # a list of multi-events indexed by timestep
    occupied = [0] * nrMev

    for i in range(0, nrMev):
        g_mev_time[i] = []
        if input_type == 1:  # CATEGORICAL
            for a in range(0, nrAttr):
                g_mev_time[i].append(randrange(0, alphaPerAttr))
        else:  # ITEM SET
            amount = randrange(0, nrAttr)  # per timestep choose the number of attributes that are present
            mylist = range(0, nrAttr)
            sample = [mylist[i] for i in sorted(random.sample(range(len(mylist)), amount))]
            for a in sample:
                g_mev_time[i].append(a)
        occupied[i] = [False] * len(g_mev_time[i])

    tijd = datetime.now().strftime("%d-%m-%Y_%H.%M.%S.%f")
    p_file = None
    # insert patterns and write pattern file
    if patterns is not None:  # zero patterns
        p_file = 'patterns_' + tijd + '_'
        cnt = 1
        while os.path.isfile(p_file + str(cnt) + '.txt'):  # to avoid overwriting
            cnt = cnt + 1
        p_file = p_file + str(cnt) + '.txt'
        with open(p_file, 'w') as fout:
            fout.write(str(input_type) + ' ' + str(len(patterns)) + ' ' + str(nrAttr) + ' ' + str(alphaPerAttr) + '\n')
            for pattern in patterns:
                sup = insertPattern(g_mev_time, nrMev, pattern, overwrite, interleave, occupied)
                if sup < minsup:
                    minsup = sup
                fout.write(
                    str(pattern.size) + ' ' + str(pattern.length) + ' ' + str(sup) + ' ' + str(pattern.gapChance) + ' ')
                for evs in pattern.eventSets:
                    fout.write(str(len(evs.events)) + ' ')
                    for ev in evs.events:
                        fout.write(str(ev.aid) + ' ' + str(ev.sym) + ' ')
                fout.write('\n')

                # print DATA to file
    d_file = 'data_' + tijd + '_'
    cnt = 1
    while os.path.isfile(d_file + str(cnt) + '.dat'):  # to avoid overwriting
        cnt = cnt + 1
    d_file = d_file + str(cnt) + '.dat'
    with open(d_file, 'w') as fout:
        fout.write(str(nrAttr) + ' ')  # header info
        for attri in range(0, nrAttr):
            fout.write(str(alphaPerAttr) + ' ')
        fout.write('\n')
        if input_type == 1:  # CATEGORICAL
            for a in range(0, nrAttr):
                for i in range(0, nrMev):
                    fout.write(str(g_mev_time[i][a]) + ' ')
                if a != nrAttr - 1:
                    fout.write('-2\n')
        else:  # ITEM SET
            for i in range(0, nrMev):
                for a in range(0, len(g_mev_time[i])):
                    fout.write(str(g_mev_time[i][a]) + ' ')
                fout.write('-2 ')
    return [d_file, p_file, minsup]


def usage():
    print('Use the following optional parameters:\n'
          '\t-h \tShow help\n'
          '\t-r \tRun Ditto (default=True)\n'
          '\t-i \t1=categorical <default>, 2=item set\n'
          '\t-p \t# of synthetic patterns to insert (default=5)\n'
          '\t-a \t# of attributes in the generated data (default=1)\n'
          '\t-m \t# of multi-events in the generated data (default=10000)\n'
          '\t-z \talphabet size for each attribute in the generated data (default=100)\n'
          '\t-o \toverwrite: 0=False <default>, 1=True\n'
          '\t-u \tunique symbols in generated patterns: 0=False <default>, 1=True\n'
          '\t-l \tinterleaving of occurrences per pattern: 0=False, 1=True <default>\n'
          '\t--minsup= \tminimal support for Ditto (default=90% of the lowest pattern-support)\n'
          '\t--minPATsup= \tminimal support per synthetic patterns in tenths of a percentage of total #events (default=10 -> 1%)\n'
          '\t--maxPATsup= \tmaximal support per synthetic patterns in tenths of a percentage of total #events (default=10 -> 1%)\n'
          '\t--mingap= \tminimal gap chance between multi-events of an occurence of a synthetic patterns (default=0.05)\n'
          '\t--maxgap= \tmaximal gap chance between multi-events of an occurence of a synthetic patterns (default=0.05)\n'
          '\t--minsz= \tminimal size of a synthetic patterns (default=5)\n'
          '\t--maxsz= \tmaximal size of a synthetic patterns (default=5)\n'
          )


def main(argv):
    interleave = True  # occurrences of a single pattern may or may not interleave
    unique = False  # every symbol in every generated pattern is unique in order to avoid patterns to block each other
    input_type = 1  # 1=CATEGORICAL
    nrOfPatterns = 5
    minsup = None  # for Ditto
    minPatSup = 10  # per pattern a percentage of the total nr of events -> in tenths of a percentage, i.e. 10=1%
    maxPatSup = 10
    minGapChance = 0.05
    maxGapChance = 0.05
    minSize = 5
    maxSize = 5
    nrAttr = 4
    nrMev = 10000
    alphPerAttr = 100
    overwrite = False
    runDitto = True

    try:
        opts, args = getopt.getopt(argv, 'l:u:r:i:p:a:m:z:o:h',
                                   ['minsup=', 'minPATsup=', 'maxPATsup=', 'minsz=', 'maxsz=', 'mingap=', 'maxgap='])
    except getopt.GetoptError as err:
        print(err)
        usage()
        sys.exit(2)

    for opt, arg in opts:
        if opt == '-h':
            usage()
            sys.exit(2)
        elif opt == '-i':
            input_type = int(arg)
        elif opt == '-l':
            interleave = bool(arg)
        elif opt == '-r':
            runDitto = bool(arg)
        elif opt == '-p':
            nrOfPatterns = int(arg)
        elif opt == '-u':
            unique = bool(arg)
        elif opt == '-a':
            nrAttr = int(arg)
        elif opt == '-m':
            nrMev = int(arg)
        elif opt == '-z':
            alphPerAttr = int(arg)
        elif opt == '-o':
            overwrite = bool(arg)
        elif opt == '--minsup':
            minsup = int(arg)
        elif opt == '--minPATsup':
            minPatSup = int(arg)
        elif opt == '--maxPATsup':
            maxPatSup = int(arg)
        elif opt == '--mingap':
            minGapChance = int(arg)
        elif opt == '--maxgap':
            maxGapChance = int(arg)
        elif opt == '--minsz':
            minSize = int(arg)
        elif opt == '--maxsz':
            maxSize = int(arg)

    if nrMev < 1 or \
            nrAttr < 1 or \
            maxPatSup > 1000 or \
            maxPatSup < minPatSup or \
            maxGapChance < minGapChance or \
            maxSize < minSize or \
            (input_type != 1 and input_type != 2):
        print('Wrong input!')
        usage()
        sys.exit(2)

    # generate the patterns
    patterns = None
    if nrOfPatterns != 0:
        patterns = generateRandomPatterns(input_type, nrOfPatterns, minPatSup, maxPatSup, minGapChance, maxGapChance,
                                          minSize, maxSize, nrAttr, alphPerAttr, nrMev, unique)
        if patterns is None:
            print('ERROR: alphabet too small to fit all unique generated patterns.')
            sys.exit(1)

    # generate the data
    result = generateData(input_type, nrMev, nrAttr, alphPerAttr, patterns, overwrite, interleave)
    d_file = result[0]
    p_file = result[1]
    if minsup is None:
        minsup = 0.9 * result[2]

    if runDitto:
        if p_file is None:
            ret = os.system('Ditto.exe -i ' + d_file)
        else:
            ret = os.system('Ditto.exe -i ' + d_file + ' -p ' + p_file + ' -m ' + str(minsup) + '.txt')
        if ret != 0:
            print('Error with Ditto.exe')
            print(ret)


if __name__ == '__main__':
    main(sys.argv[1:])
