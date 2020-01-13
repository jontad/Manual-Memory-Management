# Testing

## Unit testing
De bibliotek vi använder inklusive den nya koden vi skrivit testas i CUnit-tester som körs med Valgrind. Vi testar för det mesta gränsfall.

## Integration testing
Den nya koden använder sig av hashtabellen och länkade listan som vi skapade i inlupp1. Att integrationen fungerar testas i CUnit-tester som körs med Valgrind. Integrationstesterna skulle behöva utökas men pga tidsbegränsningar.

## Regression testing
Testerna kör automatiskt via ett continous integration workflow i Github vid varje pull request. På det sättet upptäcker vi snabbt ifall någon funktionalitet gått sönder. Ifall testerna inte går igenom blir inte pull requesten godkänd.

# Code coverage
