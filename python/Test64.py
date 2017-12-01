#!/usr/local/bin/python
import base64
import sys

def Base64EncodeFile(inFileName, outFileName):
    fin = open(inFileName, 'rb')
    fout = open(outFileName, 'wb')
    base64.encode(fin, fout)
    fin.close()
    fout.close()

def Base64DecodeFile(inFileName, outFileName):
    fin = open(inFileName, 'rb')
    fout = open(outFileName, 'wb')
    base64.decode(fin, fout)
    fin.close()
    fout.close()

def Base64EncodeStr(s):
    return base64.standard_b64encode(s)

def Base6DecodeStr(s):
    return base64.standard_b64decode(s)
    
def TestEncode():
    Base64EncodeFile('b.txt', 'c.txt')
    print 'Encode sucess'
    Base64DecodeFile('c.txt', 'd.txt')
    print 'Decode sucess'
    f = file('b.txt', 'rb')
    f.seek(0,2)
    size = f.tell();
    print 'size = %d' % size
    f.seek(0,0)
    content = f.read(size)
    print 'len = %d' % len(content)
    print '%s' % Base64EncodeStr(content)
    
def MyHelp():
    print 'Usage: Test64.py [opt] [...]'
    print '-c Encode base64'
    print '-d Decode base64'
    print "Example: Test64.py -c 'aaaa' Test64.py -d 'aaaa' Test64.py -c file1 file2"

def Format(argv, opt = '-c'):
    ListFormat = [opt]
    for s in argv[1:]:
        if s[0] == '-':
            if len(s) == 2:
               ListFormat[0] = s
        else:
            ListFormat.append(s)
    return ListFormat

def ReadContent(fileName):
    f = file(fileName, 'rb')
    f.seek(0,2)
    size = f.tell();
    f.seek(0,0)
    return f.read(size)

def Encode0():
    s = raw_input()
    print '%s' % Base64EncodeStr(s)

def Encode1(fileName):
    content = ReadContent(fileName)
    print '%s' % Base64EncodeStr(content)

def Encode2(inFileName, outFileName):
    Base64EncodeFile(inFileName, outFileName)

def Decode0():
    s = raw_input()
    print '%s' % Base6DecodeStr(s)
    
def Decode1(fileName):
    content = ReadContent(fileName)
    print '%s' % Base6DecodeStr(content)
    
def Decode2(inFileName, outFileName):
    Base64DecodeFile(inFileName, outFileName)
    
def main():
    ListFormat = Format(sys.argv)
    nSize = len(ListFormat)
    if nSize > 3:
        MyHelp()
        return;
    if ListFormat[0] == '-c':
        if nSize == 1:
            Encode0()
        elif nSize == 2:
            Encode1(ListFormat[1])
        else:
            Encode2(ListFormat[1], ListFormat[2])
    elif ListFormat[0] == '-d':
        if nSize == 1:
            Decode0()
        elif nSize == 2:
            Decode1(ListFormat[1])
        else:
            Decode2(ListFormat[1], ListFormat[2])
    else:
        MyHelp()
 
if __name__=='__main__':
    main()
