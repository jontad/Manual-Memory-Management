# Testing

## Unit testing
De bibliotek vi använder inklusive den nya koden vi skrivit testas i CUnit-tester som körs med Valgrind. Vi testar för det mesta gränsfall.

## Integration testing
Den nya koden använder sig av hashtabellen och länkade listan som vi skapade i inlupp1. Att integrationen fungerar testas i CUnit-tester som körs med Valgrind. Integrationstesterna skulle behöva utökas men pga tidsbegränsningar.

## Regression testing
Testerna kör automatiskt via ett continous integration workflow i Github vid varje pull request. På det sättet upptäcker vi snabbt ifall någon funktionalitet gått sönder. Ifall testerna inte går igenom blir inte pull requesten godkänd.

# Code coverage
##### allocate.c
- line coverage: 97.6%
- functions: 100%
##### cascade.c
- line coverage: 100%
- functions: 100%
##### cleanup.c
- line coverage: 93.9%
- functions: 83.3%

# Most nasty bugs
_Kommentar: Vi skapade inte issues i Github när vi upptäckte buggar och har därför svårt att komma ihåg de värsta buggarna vi stötte på._

En svårlöst bugg som vi stötte på när vi implementerade en bit array för att kunna veta ifall en pekare blivit allokerad av vårt program eller inte var att vi inte kunde komma ett bra sätt att hasha pekarna. Ifall man delar pekarvärdet som ska stoppas in i bit arrayen på antalet rader i bit arrayen skulle man kunna garantera att alla pekare får en egen unik plats i bit arrayen. För att det ska fungera behöver information kring hur stort intervall pekarvärdet kan hamna i. Annars kan inte en tillräckligt stor bit array skapas. Väljer vi att skapa en bit array med 512 rader kommer en pekare p med värdet 1 000 000 inte hamna i bit arrayen då 1 000 000/512 > 512. Har vi koll på minsta möjliga värdet och högsta möjliga värdet en pekare kan få när programmet körs så kan vi lösa detta. Utan den informationen är vi dessvärre tvungna att hasha pekaren på annat vis, vi valde då att bestämma radnumret mha p%512 istället. Det har dock visat sig att denna implementation inte är så bra då det är förhållandevis vanligt att två pekare hashas till samma bit.
