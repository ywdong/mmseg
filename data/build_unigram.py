#/usr/bin/python
# -*- coding: utf-8 -*-
import sys
import unicodedata
import re
import codecs
import os
import glob

'''
在segmenter_token.h中的定义是：
class TokenType {
enum type {
  TYPE_COMMON = 0,
  TYPE_ORG  = 1 << 0,
  TYPE_PERSONAL_NAME = 1 << 1,
  TYPE_PLACE = 1 << 2,
  // TODO: add more types if needed
};
};
'''

def TypeStringToInt(type_str):
    if type_str == 'TYPE_COMMON':
        return 0
    elif type_str == 'TYPE_ORG':
        return 2 ** 0
    elif type_str == 'TYPE_PERSONAL_NAME':
        return 2 ** 1
    elif type_str == 'TYPE_PLACE':
        return 2 ** 2
    else:
        # return common type as default
        return 0

def GetTypeFromFileName(fname):
    if fname == 'org.txt':
        return TypeStringToInt('TYPE_ORG')
    elif fname == 'personal_name.txt':
        return TypeStringToInt('TYPE_PERSONAL_NAME')
    elif fname == 'place.txt':
        return TypeStringToInt('TYPE_PLACE')
    else:
        print 'bad file,', fname
        return None

def TokenTypesToInt(types):
    ret = 0;
    for type in types:
        ret += TypeStringToInt(type)
    return ret

def LoadFile(infile, token_type):
    print "current file is: " + infile
    filename = os.path.basename(infile)
    print 'filename:', filename
    type = GetTypeFromFileName(filename)
    fp = codecs.open(infile, "r", "UTF-8")
    lines = fp.readlines()
    fp.close()
    print "line number:", len(lines)
    for line in lines:
        line = line.strip()
        print line, type
        if line in token_type:
            token_type[line] += type
        else:
            token_type[line] = type
    print "Load file done:", infile

def main():
    print 'Usage:%s char.stat.txt Lexicon_full_words.txt output' % sys.argv[0]
    # load single word
    fh = codecs.open('char.stat.txt', "r", "UTF-8")
    lines = fh.readlines()
    fh.close()
    uni_char = {}
    token_type = {}
    for l in lines:
        l = l.strip()
        toks = l.split('\t')
        k = toks[0]
        cnt = int(toks[1])
        if k not in uni_char:
            uni_char[k] = cnt

    # load tokens
    fh = codecs.open('Lexicon_full_words.txt', "r", "UTF-8")
    lines = fh.readlines()
    fh.close()
    for l in lines:
        l = l.strip()
        if l not in uni_char:
            uni_char[l] = 1

    token_type = {}
    # Load all dicts of all token type
    type_dir = 'token_type/'
    for infile in glob.glob(os.path.join(type_dir, '*.txt')):
        LoadFile(infile, token_type)

    for k in token_type:
        print '%s\t%d' % (k, token_type[k])

    # Save to unigram.txt
    fo = open('unigram.txt', 'w')
    for k in uni_char:
        if k in token_type:
            type = token_type[k]
        else:
            type = 0
        record = str("%s\t%d\t%d\n" % (k, uni_char[k], type))
        fo.write(record.encode('UTF-8'))
    print "done!"

if __name__ == "__main__":
    main()
