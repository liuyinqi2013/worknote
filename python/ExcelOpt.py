import xlrd

def SheetRead(sheet):
    rowCount = sheet.nrows
    colCount = sheet.ncols
    print 'sheetName = %s' % (sheet.name)
    print 'rowCount = %d ,colCount = %d' % (rowCount, colCount)
    for r in range(rowCount):
        row = []
        for c in range(colCount):
            print sheet.cell_value(r, c)
    print '-------------------------------------------------------------'

def TestExcelRead(excelName):
    bk = xlrd.open_workbook(excelName);
    sheetRange = range(bk.nsheets)
    print 'sheet count = %d' % bk.nsheets
    for i in sheetRange:
        SheetRead(bk.sheet_by_index(i))
    
if __name__ == '__main__':
    TestExcelRead('panda.xlsx')
