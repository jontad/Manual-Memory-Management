Vi försökte skriva kod som dokumenterade sig själv. Vad vi menar är
att variabelnamnen, funktionsnamnen och struktnamnen var så
tydliga som möjligt så att en okänd användare skulle kunna föstå
vad koden gör. Vi följde "GNU coding standard" så gott det gick,
men med vissa undantag. T.ex så skrev vi ej funktionsnamn på nya rader som standarden vill.  

Sedan såg vi till att gruppera relevant kod med varandra så att det
var lättare att läsa. Vi gjorde inga tidiga optimeringar eftersom
vi var rädda att koden skulle bli svårläslig.

När det väl gäller underhållbarheten så har läsbarheten gjort
det möjligt för oss att underhålla koden utan att behöva ägna för
mycket tid på att läsa och förstå koden. Detta märktes som mest
när vi hade gjort lite fel med release och retain funktionerna i
inlupp 2 filerna och kunde ganska snabbt lösa de felen tack vore	
detta.

Koden var lätt att testa, vi fokuserade väldigt mycket på edge-cases
pga. den tidsramen vi hade att jobba med. 