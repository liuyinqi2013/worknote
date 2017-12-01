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
    url = r'http://bd.tfb8.com/cgi-bin/v1.0/order_fuzzy_query.cgi'
    #url = r'http://192.168.0.152:8080/empPlatform/public/status'
    headData = {'Cookie':'userId=106;uin=1000000068;sid=1234;user_type=2;JSESSIONID=CAE00AB454A40593E47BC0A5230520B1'}
    postData = {'user' : 'laoda', 'password' : '123'}
    getData = {'req_type' : '1', 'sub_oper_type' : '1', 'userId':'106','password' : 'fcea920f7412b5da7be0cf42b8c93759','new_password' : '202cb962ac59075b964b07152d234b70'}
    CallCGI(url, getData, postData, headData)

if __name__ == '__main__':
    main() 
    

