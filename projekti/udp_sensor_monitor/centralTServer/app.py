#!/usr/bin/env python
import os
import tornado.ioloop
import tornado.web
import tornado.websocket
import tornado.httpclient
import json
import urllib


klijenti = set()
klasteri = set()

class MainHandler(tornado.web.RequestHandler):
    def initialize(self):
        pass

    def get(self):
        self.render("home.html") 
       

    def post(self):
        nodeid = self.get_argument('nodeid','')
        adresa = self.get_argument('adresa','')
        osvetljenje = self.get_argument('osvetljenje','')
        temperatura = self.get_argument('temperatura','')
        baterija = self.get_argument('baterija','')
        vreme = self.get_argument('vreme','')
        klasteri.add(self.request.remote_ip)
        #klasteri.add(self.request.headers['X-Client-Ip'])
    
        self.write("Stiglo ")
        for klijent in klijenti:
            klijent.write_message(json.dumps({"nodeid":nodeid,"adresa":adresa, "temperatura":temperatura, "osvetljenje":osvetljenje,"baterija":baterija, "vreme":vreme}))
            self.write


def handle_request(response):
        if response.error:
            print "Error:", response.error
        else:
            print response.body

class SocketHandler(tornado.websocket.WebSocketHandler):
    def open(self):
        klijenti.add(self)
        self.write_message(json.dumps({"debug":"konektovan"}))
        
    def on_message(self, message):
        self.write_message(json.dumps({"debug":"poruka"}))
        if (len(klasteri)):
            body = urllib.urlencode({"message":message})
            for klaster in klasteri:
                self.write_message(json.dumps({"klaster":klaster}))
                print "Saljem: ", body, " na: ",klaster+":8888"
                http_client = tornado.httpclient.AsyncHTTPClient()
                http_client.fetch("http://"+klaster+":8888", handle_request, method='POST', headers=None, body=body) 


    def on_close(self):
        klijenti.remove(self)



settings = {
    "template_path": os.path.join(os.path.dirname(__file__), "templates"),
    "static_path": os.path.join(os.path.dirname(__file__), "static"),
    "debug" : True
}

image_path = os.path.join(os.path.dirname(__file__), "static/img")

application = tornado.web.Application([
    (r"/", MainHandler),
    (r"/ws", SocketHandler),
    (r"/img/(.*)",tornado.web.StaticFileHandler, {"path": image_path} )
],**settings)



def main(address):
    
    application.listen(8080, address)
    tornado.ioloop.IOLoop.instance().start()
    

if __name__ == "__main__":
    address = "127.0.0.1"
    main(address)
