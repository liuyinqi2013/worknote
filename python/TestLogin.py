import urllib
import urllib2

def CallCGI(url, getData = {}, postData = {}, headData = {}):
    getString  = urllib.urlencode(getData)
    postString = urllib.urlencode(postData)
    getUrl = url
    if len(getString) > 0:
        getUrl = url + '?' + getString
    request = urllib2.Request(getUrl, postString, headData)
    respone = urllib2.urlopen(request)
    print 'page = ', respone.read()


def main():
    url = r'http://192.168.17.123:8080/empPlatform/public/login'
    headData = {'Cookie':'userId=100;uin=1000000068;sid=1234;user_type=2'}
    postData = {'loginName' : 'Ð¤ÔÆ', 'password' : '111111', 'securityCode' : '111111'}
    getData = {'req_type' : '1', 'sub_oper_type' : '1'}
    CallCGI(url, getData, postData, headData)

if __name__ == '__main__':
    main() 
    

