import MySQLdb
import re

class InterfaceItem:
    def __init__(self, inter_id, inter_state, inter_name, inter_desc, user_id):
        self.inter_id    = inter_id
        self.inter_state = inter_state
        self.inter_name  = inter_name
        self.inter_desc  = inter_desc
        self.user_id     = user_id

    def __str__(self):
        return self.toStr('|')
    
    def toStr(self, sep = ' '):
        seq = (self.inter_id, self.inter_state, self.inter_name, self.inter_desc, self.user_id)
        return sep.join(seq)
    
    def toInsertSql(self):
        sql = " INSERT INTO company_permission_db.t_interface_define(Finterface_id, Fstate, Fname, Fdesc, Fcreate_time, Fmodify_time, Flast_modify_user) \
VALUES(%s, %s, '%s', '%s', now(), now(), '%s');"%(self.inter_id, self.inter_state, self.inter_name, self.inter_desc, self.user_id)
        return sql
    
    def toUpdateSql(self):
        sql = " UPDATE company_permission_db.t_interface_define SET Fstate = %s, Fname = '%s', Fdesc = '%s', Fmodify_time = now, Flast_modify_user = '%s' WHERE Finterface_id = %s;"\
              %(self.inter_state, self.inter_name, self.inter_desc, self.user_id, self.inter_id)
        return sql
    
    def toDeleteSql(self):
        sql = " DELTE FROM company_permission_db.t_interface_define  WHERE Finterface_id = %s;"%(self.inter_id)
        return sql

    def toSelectSql(self):
        sql = " SELECT * FROM company_permission_db.t_interface_define  WHERE Finterface_id = %s;"%(self.inter_id)
        return sql
        

class PermissonItem:
    def __init__(self, perm_id, perm_state, perm_name, perm_desc, user_id):
        self.perm_id    = perm_id
        self.perm_state = perm_state
        self.perm_name  = perm_name
        self.perm_desc  = perm_desc
        self.user_id    = user_id

    def __str__(self):
        return self.toStr('|')

    def toStr(self, sep = ' '):
        seq = (self.perm_id, self.perm_state, self.perm_name, self.perm_desc, self.user_id) 
        return sep.join(seq)
    
    def toInsertSql(self):
        sql = " INSERT INTO company_permission_db.t_permission_define(Fpermission_id, Fstate, Fname, Fdesc, Fcreate_time, Fmodify_time, Flast_modify_user) \
VALUES('%s', %s, '%s', '%s', now(), now(), '%s');"%(self.perm_id, self.perm_state, self.perm_name, self.perm_desc, self.user_id)
        return sql

    def toUpdateSql(self):
        sql = " UPDATE company_permission_db.t_permission_define SET Fstate = %s, Fname = '%s', Fdesc = '%s', Fmodify_time = now(), Flast_modify_user = '%s' WHERE Fpermission_id = '%s';"\
              %(self.perm_state, self.perm_name, self.perm_desc, self.user_id, self.perm_id)
        return sql
    
    def toDeleteSql(self):
        sql = " DELTE FROM company_permission_db.t_permission_define  WHERE Fpermission_id = %s;"%(self.perm_id)
        return sql

    def toSelectSql(self):
        sql = " SELECT * FROM company_permission_db.t_permission_define  WHERE Fpermission_id = %s;"%(self.perm_id)
        return sql
    
class InterPermItem:
    def __init__(self, inter_item, perm_item, user_id):
        self.inter_item = inter_item
        self.perm_item  = perm_item
        self.user_id    = user_id
        
    def __str__(self):
        return self.toStr('|')
    
    def toStr(self, sep = ' '):
        seq = (self.inter_item.toStr(sep), sself.perm_item.toStr(sep), self.user_id) 
        return sep.join(seq)

    def toSqls(self):
        sql1 = " INSERT INTO company_permission_db.t_permission_interface_set(Fpermission_id, Finterface_id, Fstate, Fcreate_time, Fmodify_time, Flast_modify_user) \
VALUES('%s', %s, 1, now(), now(), '%s');"%(self.perm_item.perm_id, self.inter_item.inter_id, self.user_id)
        sql2 = " INSERT INTO company_permission_db.t_permission_interface_set_log(Foper_type, Fpermission_id, Finterface_id, Fstate, Fmodify_time, Flast_modify_user) \
VALUES(1, '%s', %s, 1, now(), '%s');"%(self.perm_item.perm_id, self.inter_item.inter_id, self.user_id)
        seq = [sql1, sql2]
        return seq
              
class InitPermissonDB:
    def __init__(self):
        self.interMap = {}
        self.permMap = {}
        self.ipList = []
        self.dbConn = MySQLdb.connect('192.168.0.115', 'depuser', 'depuser')
        self.cursor = self.dbConn.cursor()
        self.cursor.execute("set names utf8")
        self.dbConn.commit()

    def __del__(self):
        self.dbConn.close()

    def execute(self):
        #self.readConfig('dbconf.txt')
        #self.readConfig('commdb.txt')
        self.readConfig('commdb_other.txt')
        self.createSQLScript('SQLScript.sql')
        #self.clearDB()
        #self.insertDB()
        
    def readConfig(self,fileName):
        fd = file(fileName, 'r')
        while 1:
            line = fd.readline()
            if not line:
                break
            strlist = re.split(r'\s+', line)
            if line[0] == '#' or len(strlist) < 9:
                continue
            pitem = PermissonItem(strlist[0].strip(), strlist[1].strip(), strlist[2].strip(), strlist[3].strip(), strlist[8].strip())
            iitem = InterfaceItem(strlist[4].strip(), strlist[5].strip(), strlist[6].strip(), strlist[7].strip(), strlist[8].strip())
            ipitem = InterPermItem(iitem, pitem, strlist[8].strip())
            self.permMap[pitem.perm_id] = pitem
            self.interMap[iitem.inter_id] = iitem
            self.ipList.append(ipitem)

    def insertDB(self):
        self.insertInterItem()
        self.insertPermItem()
        self.insertIPItem()
        
    def insertInterItem(self):
        self.excuteItemMap(self.permMap)
        
    def insertPermItem(self):
        self.excuteItemMap(self.interMap)
        
    def insertIPItem(self):
        for item in self.ipList:
            seq = item.toSqls()
            try:
                for SQL in seq:
                    SQL = SQL.decode('gb2312').encode('utf8')
                    print 'sqls = ',SQL
                    self.cursor.execute(SQL)
                self.dbConn.commit()
            except MySQLdb.ProgrammingError, x:
                print 'ProgrammingError : ', x
                self.dbConn.rollback()
            except MySQLdb.DatabaseError, x:
                print 'DatabaseError : ', str(x)
                self.dbConn.rollback()

    def excuteItemMap(self, itemMap):
        for (key, value) in itemMap.items():
            SQL = value.toInsertSql().decode('gb2312').encode('utf8')
            self.executeSQL(SQL)

    def createSQLScript(self, fileName):
        fd = file(fileName, 'w')
        fd.writelines("/*---------------------------------接口SQL------------------------------------------*/\n")
        for (key, value) in self.interMap.items():
            SQL = value.toInsertSql()
            #SQL = value.toInsertSql().decode('gb2312').encode('utf8')
            fd.writelines(SQL + '\n')
        fd.writelines('\n\n')    
        fd.writelines("/*---------------------------------权限SQL---------------------------------------------*/\n")
        for (key, value) in self.permMap.items():
            SQL = value.toInsertSql()
            #SQL = value.toInsertSql().decode('gb2312').encode('utf8')
            fd.writelines(SQL + '\n')
        fd.writelines('\n\n')
        fd.writelines("/*---------------------------------接口权限映射SQL--------------------------------------*/\n")
        for item in self.ipList:
            seq = item.toSqls()
            fd.writelines('\n'.join(seq))
            fd.writelines('\n')
        fd.close()

    def clearDB(self):
        SQL = "DELETE FROM company_permission_db.t_interface_define;"
        self.executeSQL(SQL)
        SQL = "DELETE FROM company_permission_db.t_permission_define;"
        self.executeSQL(SQL)
        SQL = "DELETE FROM company_permission_db.t_permission_interface_set;"
        self.executeSQL(SQL)
        SQL = "DELETE FROM company_permission_db.t_permission_interface_set_log;"
        self.executeSQL(SQL)

    def executeSQL(self, SQL):
        print 'sql = ',SQL
        try:
            self.cursor.execute(SQL)
            self.dbConn.commit()
        except MySQLdb.ProgrammingError, x:
            print 'ProgrammingError : ', x
            self.dbConn.rollback()
        except MySQLdb.DatabaseError, x:
            print 'DatabaseError : ', str(x)
            self.dbConn.rollback()
         
def main():
    #permItemList = ReadConf('dbconf.txt')
    #InsertDB(permItemList)
    #CreateSQLScript(permItemList)
    handle = InitPermissonDB()
    handle.execute()
        
if __name__=='__main__':
    main()
