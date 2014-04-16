var pdO, pdT;
var tmp = {};
          
function dodaj(msg){
    $('.new').removeClass('new');
    var ajdi = "adr"+msg.adresa.replace(/:/g,"");
    t = new Date(msg.vreme);
    strVreme = t.toLocaleTimeString();
    if (!(msg['adresa'] in tmp)) {
        $('#log tr:first').clone().attr('id',ajdi).appendTo('#log');
        tmp[msg.adresa] = {"osvetljenje":[],"temperatura":[], "nodeid":msg.nodeid};
    } 
    
    needUpdate = $('#'+ajdi).addClass('new').children();
    needUpdate.eq(0).text(msg.nodeid);
    needUpdate.eq(1).text(msg.adresa);
    needUpdate.eq(2).text(msg.osvetljenje);
    needUpdate.eq(3).text(msg.temperatura);
    needUpdate.eq(4).text(msg.baterija);
    needUpdate.eq(5).text(strVreme);
    //tableColor('#log');

    tmp[msg.adresa]["osvetljenje"].push([t, msg.osvetljenje]); 
    tmp[msg.adresa]["temperatura"].push([t, msg.temperatura]); 
}

function crtaj(selector, osvetljenje, temperatura){
    $.plot(selector, [{data:osvetljenje, label:"Osvetljenje"},{data:temperatura, label:"Temperatura", yaxis: 2}], {
			xaxis: { mode: "time", ticks: 3 },
			yaxes: [ { alignTicksWithAxis: 1, max:1024 }, {
					alignTicksWithAxis: 1 ,
					position: "right",
                    max:60
				} ]
		});
}

function tableColor(selector){
    redovi = $(selector).find('tr');
    novi = redovi.filter('.new');
    korak = 1.00/(redovi.length - 1);
    redovi.each(function  () {
        providnost = parseFloat($(this).attr('providnost'));
        if (providnost < korak) { providnost = 0; }
        else {providnost -= korak; }
        $(this).attr('providnost', providnost);
        new_bc = 'rgba(252,252,0,'+ providnost + ')';
        $(this).css('background-color', new_bc );

    });
    novi.css('background-color','rgba(252,252,0,1)');
    novi.attr('providnost','1');
}

function grafTabRefresh(){
    for (var addr in tmp){
        var light = tmp[addr]["osvetljenje"];
        var temp = tmp[addr]["temperatura"];
        var nodeid = tmp[addr]["nodeid"];
        if ($('#n'+ nodeid).length == 0){
            $('#grafPanel').append('<div class="grafPanelCell"><h2>NODE ID:'+ nodeid +'</h2><div id="n'+ nodeid +'" style="width:400px; height:200px;"></div></div>');
        }
        crtaj(('#n' + nodeid), light, temp);
    }
}

$(document).ready(function(){
    //var ws = new WebSocket("ws://tornado-kolibri.rhcloud.com:8000/ws");
    var ws = new WebSocket("ws://127.0.0.1:8080/ws"); //test
    ws.onopen = function() {
        console.log('Konektovan..');
    };
    ws.onmessage = function (evt) {
        msg =JSON.parse(evt.data);
        console.log(msg);
        //$('#adresa').text(msg['adresa']);
        //$('#svetlo').text(msg['osvetljenje']);
        //$('#temp').text(msg['temperatura']);
        dodaj(msg);
        if ((pdO != 'undefined') && (pdT != 'undefined')){
            crtaj("#grafik", pdO, pdT);
            grafTabRefresh();
        }	
    }  
    ws.onclose = function() {
        console.log('Diskonektovan!');
    };
    
    $('#log').on('click','tr',function(){
        var nodeid=$(this).find("td").eq(0).text();
        var ipaddr=$(this).find("td").eq(1).text();
        pdO = tmp[ipaddr]["osvetljenje"];
        pdT = tmp[ipaddr]["temperatura"];
        $("#grafContainer h2").html("NODE ID:" + nodeid);
        $("#grafContainer").show(100);
        crtaj("#grafik", pdO, pdT);
    });

    $('#grafContainer ').on('click', '.close',function(){
        $('#grafContainer').hide(100);
    });
    
    $('#tabs').on('click','a',function(){
        var tab = $(this).attr('page');
        $('#tabs a').removeClass('active');
        $(this).addClass('active');
        $('#pages > div').hide();
        $('#'+tab).show();
    });

    $('.podesavanja').on('click','a.primeni',function(){
        var podesavanja = $(this).parent('.podesavanja');
        var period = podesavanja.find('#period').val();
        ws.send('p'+period+'b'); 
    });
    
    $('.podesavanja').on('click','#repair',function(){
        ws.send('repair');
    });

});
