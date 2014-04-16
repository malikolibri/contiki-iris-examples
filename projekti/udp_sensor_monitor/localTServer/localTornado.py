#!/usr/bin/env python
import tornado.ioloop
import tornado.web
import tornado.httpclient
import math
import socket
import datetime
import urllib

def InCelsius(tADC):
    Rt = (10000 * (1024 - tADC))/tADC
    K = 1 / (0.001010024 + 0.000242127 * math.log(Rt) + 0.000000146 * (math.log(Rt) ** 3))
    return K - 273.15

class MainHandler(tornado.web.RequestHandler):
    def get(self):
        self.write("Lokalni Tornado Server")
    def post(self):
        data = self.get_argument('message', 'No data received')
        print data
        if (data == 'repair'):
            tcp_sock = socket.socket(socket.AF_INET6, socket.SOCK_STREAM)
            tcp_sock.connect((koreni_cvor_addr, 3333))
            tcp_sock.send('R')
            tcp_sock.close()
        else:
            print koreni_cvor_addr
            udp_sock.sendto(data, (koreni_cvor_addr, 5555))

def handle_input(fd, events):
    (data, source_addr) = udp_sock.recvfrom(4096)

    if (data == 'prijava'):
        global koreni_cvor_addr
        koreni_cvor_addr = source_addr[0]
        print data
        tcp_sock = socket.socket(socket.AF_INET6, socket.SOCK_STREAM)
        tcp_sock.connect((koreni_cvor_addr, 3333))
        tcp_sock.send('O')
        tcp_sock.close()

    else:
        adresa = source_addr[0]
        node_id = ord(data[0])    
        osvetljenje = ((ord(data[1])<<8) + ord(data[2]))/4.00
        temperatura = ((ord(data[3])<<8) + ord(data[4]))/4.00
        baterija = (ord(data[5])<<8) + ord(data[6])
        temperatura = str(round(InCelsius(temperatura),2))
        baterija = (1.223 * 1024)/baterija
        vreme = datetime.datetime.today()
        
        payload = {
            'nodeid':node_id,
            'adresa':adresa,
            'osvetljenje': osvetljenje,
            'temperatura':temperatura,
            'baterija':baterija,
            'vreme':vreme.ctime()}
        
        print "{:^5} <- Node ID <{}>".format(node_id, vreme.ctime())
        print "Adresa posiljaoca: {}".format(adresa)
        print "Osvetljenje: {:^6} Temperatura: {:^5}".format(osvetljenje, temperatura)
        print "Stanje baterije[V]: {:^5}".format(baterija)
        print "-------------------------------------------"
            
        http_client.fetch(cs_url, callback = None, method='POST', headers=None, body=urllib.urlencode(payload))


application = tornado.web.Application([
    (r"/", MainHandler),
], debug = True)

if __name__ == "__main__":
    udp_sock = socket.socket(socket.AF_INET6, socket.SOCK_DGRAM)
    udp_sock.bind(('', 5555))
    udp_sock.setblocking(0)
    koreni_cvor_addr = None
    
    http_client = tornado.httpclient.AsyncHTTPClient()
    cs_url = 'http://127.0.0.1:8080/' #Central server URL
    #cs_url = 'http://tornado-kolibri.rhcloud.com/'
    application.listen(8888)    
    io_loop = tornado.ioloop.IOLoop.instance()
    io_loop.add_handler(udp_sock.fileno(), handle_input, io_loop.READ)
    io_loop.start()
