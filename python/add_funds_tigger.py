#!/usr/bin/python
import sys,os
import urllib2
import MySQLdb
import time
import ConfigParser
import logging

class MyLog:
    def __init__(self, fileName = 'mylog.log'):
        self.logger = logging.getLogger()
        self.logger.setLevel(logging.INFO)
        self.fileHandler = logging.FileHandler(fileName, mode = 'w')
        self.fileHandler.setLevel(logging.INFO)
        formatter = logging.Formatter('%(asctime)s - %(filename)s[line:%(lineno)d] - %(levelname)s: %(message)s')
        self.fileHandler.setFormatter(formatter)
        self.logger.addHandler(self.fileHandler)
        
    def writeLog(self, message):
        self.logger.info(message)
    
class Conf:
    def __init__(self, fileName = 'conf,ini'):
        self.gLogFileName = 'increase_amount.log'
        self.gDbHost = '192.168.0.115' 
        self.gDbPort = 3306
        self.gDbName = 'bank_direct_link_db'
        self.gUserName = 'depuser'
        self.gPassWord = 'depuser'
        self.gCgiPath = 'http://api.gcdev.tfb8.com/cgi-bin/v2.0/'
        self.gCgiName = 'api_increase_amount.cgi'
        self.gCgiParam = ''
        self.read(fileName)

    def read(self, fileName):
        if not os.path.exists(fileName):
            print 'Not find ', fileName
            return -1
        cfg = ConfigParser.ConfigParser()
        cfg.read(fileName)
        self.gLogFileName = cfg.get('global', 'logFileName')
        self.gDbHost   = cfg.get('dbconf', 'dbHost')
        self.gDbPort   = cfg.getint('dbconf', 'dbPort')
        self.gDbName   = cfg.get('dbconf', 'dbName')
        self.gUserName = cfg.get('dbconf', 'userName')
        self.gPassWord = cfg.get('dbconf', 'passWord')
        self.gCgiPath  = cfg.get('cgiconf', 'cgiPath')
        self.gCgiName  = cfg.get('cgiconf', 'cgiName')
        self.gCgiParam = cfg.get('cgiconf', 'cgiParam')

        if self.gCgiPath[-1] != '/':
            self.gCgiPath += '/'
        return 0

    def toStr(self):
        content = 'gLogFileName : ' + self.gLogFileName + '\n'
        content += 'gDbHost : ' + self.gDbHost + '\n'
        content += 'gDbPort : ' + str(self.gDbPort) + '\n'
        content += 'gDbName : ' + self.gDbName + '\n'
        content += 'gUserName : ' + self.gUserName + '\n'
        content += 'gPassWord : ' + self.gPassWord + '\n'
        content += 'gCgiPath : ' + self.gCgiPath + '\n'
        content += 'gCgiName : ' + self.gCgiName + '\n'
        content += 'gCgiParam : ' + self.gCgiParam + '\n'
        return content

        
class Trace:
    def __init__(self):
        print '------------------------------ begin:' + self.dateTime() + '---------------------'
        
    def __del__(self):
        print '------------------------------ end:' + self.dateTime() + '---------------------'
        
    def dateTime(self):
        return time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())

class AddFuns:
    def __init__(self, confFile):
        self.conf = Conf(confFile)
        self.log  = MyLog(self.conf.gLogFileName)
        self.Print(self.conf.toStr())
        try:
            self.dbConn = MySQLdb.connect(self.conf.gDbHost, self.conf.gUserName, self.conf.gPassWord, self.conf.gDbName)
            self.cursor = self.dbConn.cursor()
        except MySQLdb.OperationalError, x:
            self.Print('Happend OperationalError exception errcode = ' + str(x.getcode()))

    def Execute(self, oper_type):
        self.Print('add Funs begin')   
        #dbResult = self.QueryDb(oper_type)
        #self.CallCgiServer1(dbResult, oper_type)
        if  oper_type == '1':
            self.SyncProc()
        else:
            self.AsynProc()
        self.Print('add Funs end')

    def AsynProc(self):
        self.Print('AsynProc begin')
        try:
            dbConn = MySQLdb.connect(self.conf.gDbHost, self.conf.gUserName, self.conf.gPassWord, 'fundsadd_db')
            cursor = dbConn.cursor()
            sql = 'SELECT Flistid FROM t_add_funds_order WHERE UNIX_TIMESTAMP(now()) - UNIX_TIMESTAMP(Fmodify_time) > 60'
            self.Print('Query SQL : ' + sql)
            cursor.execute(sql)
            for row in cursor.fetchall():
                sql = "SELECT Fid, Fspid FROM t_spdb_daytime_account_detail WHERE Fstate = 1 AND Flistid = '" + row[0] + "'"
                self.Print('Query SQL : ' + sql)
                self.cursor.execute(sql)
                self.CallCgiServer1(self.cursor.fetchall(), "2")
        except MySQLdb.OperationalError, x:
            self.Print('Happend OperationalError exception errmsg = ' + str(x))
        self.Print('AsynProc end')
            
            
    def SyncProc(self):
        self.Print('SyncProc begin')
        sql = 'SELECT Fid, Fspid FROM t_spdb_daytime_account_detail WHERE Fstate = 0 AND Fauto_flag = 1'
        self.Print('Query SQL : ' + sql)
        self.cursor.execute(sql)
        self.CallCgiServer1(self.cursor.fetchall(), "1")
        self.Print('SyncProc end')
    '''
    def QueryDb(self, oper_type):
        dbConn = MySQLdb.connect(self.conf.gDbHost, self.conf.gUserName, self.conf.gPassWord, self.conf.gDbName)
        cursor = dbConn.cursor()
        sql = 'SELECT Fid, Fspid FROM t_spdb_daytime_account_detail WHERE Fstate = 0 AND Fauto_flag = 1'
        if oper_type == '1':
            sql = 'SELECT Fid, Fspid FROM t_spdb_daytime_account_detail WHERE Fstate = 0 AND Fauto_flag = 1'
        elif  oper_type == '2':
            sql = 'SELECT Fid, Fspid FROM t_spdb_daytime_account_detail WHERE Fstate = 1 AND Fauto_flag = 1'
        else:
            sql = 'SELECT Fid, Fspid FROM t_spdb_daytime_account_detail WHERE Fstate = 0 AND Fauto_flag = 1'
        self.Print('Query SQL : ' + sql)
        cursor.execute(sql)
        return cursor.fetchall()
    '''
        
    def CallCgiServer(self, Fid, spid, oper_type):
        try:
            url = self.conf.gCgiPath + self.conf.gCgiName + '?' + 'Fid=' + Fid + '&Spid=' + spid + '&OperType=' + oper_type
            self.Print('Url : ' + url)
            res = urllib2.urlopen(url)
            self.Print('Server retrun : ' + res.read())
        except urllib2.HTTPError, x:
            self.Print('Happend HTTPError exception errmsg = ' + str(x))
        return 0

    def CallCgiServer1(self, dbResult, oper_type):
        for row in dbResult:
            self.Print(str(row[0]) + ' ' + str(row[1]))
            self.CallCgiServer(str(row[0]), str(row[1]), oper_type)
            
    def Print(self, message):
        self.log.writeLog(message)
        
def Uasge():
    print 'Usge : add_funds_tigger.py [opt]'
    print ' opt : 1 - sync process, 2 - asyn process '
    print 'default opt = 1'
    
def main():
    T = Trace();
    opt = '1'
    if len(sys.argv) < 2:
        opt = '1'
    else:
        opt = sys.argv[1]
    add = AddFuns('conf.ini')
    add.Execute(opt)
    return 0
 
if __name__ == "__main__":
    main()
        
        
