**RPL border ruter**
Rpl border ruter sluzi kao pocetna tacka (root) iz koje se granaju ostali nodovi. 
Border ruter je veza senzorske mreze sa spoljnjim svetom. U nasem slucaju podaci se salju preko serijskog port kontrolera. Podaci su spakovani u pakete koji se salju koriscenjem SLIP (Serial Line IP) protokola. 

 * Treba ucitati program boarder-router.c 
 * Nakon ucitavanja otici u tools folder i pokrenuti tunslip6 (6 je za IPv6) `sudo ./tunslip6 aaaa::1/64 -s /dev/ttyUSB1 -v6`  
  `aaaa::1/64` predstavlja prefiks koji ce imati svi nodovi  
  `/dev/ttyUSB1` je komunikacion port uredjaja (Ne moze se u isto vreme koristiti debug output)
  `-v6` nema veze sa IPv6 vec sa nivoom ispisa informacija. Sa `-v6` ispisuju se svi podaci (ukljucujuci i debug)
 * Nakon pokretanja `tunslip6` programa i naknadnog restartovanja kontrolera on ce ispisati svoju IPv6 adresu. Primer:
 ```
 Server IPv6 addresses:
 aaaa::21a:4c00:13e1:a2cd
 fe80::21a:4c00:13e1:a2cd
```
 * Koristeci prvu izlistanu adresu ( sa prefiksom aaaa ) pingujemo kontroler sa komandom `ping6 aaaa::21a:4c00:13e1:a2cd`
 
