#!/usr/bin/python
import sys,os
import urllib2
import MySQLdb
import time
import ConfigParser
import logging

confFileName = 'D:\workspace\Python\conf.ini'

gLogFileName = 'increase_amount.log'
gDbHost = '192.168.0.115' 
gDbPort = 3306
gDbName = 'bank_direct_link_db'
gUserName = 'depuser'
gPassWord = 'depuser'
gCgiPath = 'http://api.gcdev.tfb8.com/cgi-bin/v2.0/'
gCgiName = 'liuyq_demo.cgi'
gCgiParam = ''

class MyLog:
    def __init__(self, fileName = 'mylog.log'):
        self.logger = logging.getLogger()
        self.logger.setLevel(logging.INFO)
        self.fileHandler = logging.FileHandler(fileName, mode = 'w')
        self.consoleHandler = logging.StreamHandler()
        self.fileHandler.setLevel(logging.INFO)
        self.consoleHandler.setLevel(logging.INFO)
        formatter = logging.Formatter('%(asctime)s - %(filename)s[line:%(lineno)d] - %(levelname)s: %(message)s')
        self.fileHandler.setFormatter(formatter)
        self.consoleHandler.setFormatter(formatter)
        self.logger.addHandler(self.fileHandler)
        self.logger.addHandler(self.consoleHandler)
        
    def writeLog(self, message):
        self.logger.info(message)

gLoger = MyLog(gLogFileName)

def Print(message):
    gLoger.writeLog(message)

def readIni(fileName):
    if not os.path.exists(confFileName):
        print 'Not find ', confFileName
        return -1
    cfg = ConfigParser.ConfigParser()
    cfg.read(fileName)
    gLogFileName = cfg.get('global', 'logFileName')
    gDbHost   = cfg.get('dbconf', 'dbHost')
    gDbPort   = cfg.getint('dbconf', 'dbPort')
    gDbName   = cfg.get('dbconf', 'dbName')
    gUserName = cfg.get('dbconf', 'userName')
    gPassWord = cfg.get('dbconf', 'passWord')
    gCgiPath  = cfg.get('cgiconf', 'cgiPath')
    gCgiName  = cfg.get('cgiconf', 'cgiName')
    gCgiParam = cfg.get('cgiconf', 'cgiParam')

    if gCgiPath[-1] != os.sep:
        gCgiPath += os.sep
        
    Print('gLogFileName : ' + gLogFileName)    
    Print('gDbHost : ' + gDbHost)
    Print('gDbPort : ' + str(gDbPort))
    Print('gDbName : ' + gDbName)
    Print('gUserName : ' + gUserName)
    Print('gPassWord : ' + gPassWord)
    Print('gCgiPath : ' + gCgiPath)
    Print('gCgiName : ' + gCgiName)
    Print('gCgiParam : ' + gCgiParam)
    
    return 0

def queryDb():
    dbConn = MySQLdb.connect(gDbHost, gUserName, gPassWord, gDbName)
    cursor = dbConn.cursor()
    #sql = 'SELECT Fid,spid FROM t_spdb_daytime_account_detail WHERE Fstate = 0 AND Fauto_flag = 1
    #sql = 'SELECT VERSION()'
    sql = 'SELECT Fid,Faccount FROM t_spdb_daytime_account_detail WHERE Fid = 1173'
    Print('Query SQL : ' + sql)
    cursor.execute(sql)
    return cursor.fetchall()
 
def callServer():
    url = gCgiPath + gCgiName
    res = urllib2.urlopen(url)
    Print('Url : ' + url)
    Print('Server retrun : ' + res.read())
    return 0

def callServer1(Fid, spid):
    url = gCgiPath + gCgiName + '?' + 'Fid = ' + Fid + '&spid = ' + spid 
    res = urllib2.urlopen(url)
    Print('Url : ' + url)
    Print('Server retrun : ' + res.read())
    return 0

def callServer2(dbResult):
    for row in dbResult:
        Print(str(row[0]) + ' ' + str(row[1]))
    callServer1(str(row[0]), str(row[1]))

class Trace:
    def __init__(self):
        Print('------------------------------ begin:' + self.dateTime() + '---------------------')
        
    def __del__(self):
        Print('------------------------------ end:' + self.dateTime() + '---------------------')
        
    def dateTime(self):
        return time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())
    
def main():
    gLoger = MyLog(gLogFileName)
    T = Trace();
    ret = readIni(confFileName)
    if -1 == ret:
        Print('Read config file failed')
        return -1
    result = queryDb()
    callServer2(result)
    '''print 'Query result : ',result
    for row in result:
        print row[0]
    callServer()'''
    return 0
 
if __name__ == "__main__":
    main()
        
        
